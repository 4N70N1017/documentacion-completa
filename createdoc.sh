#!/bin/bash

# Carpeta de logs
LOGDIR="Logs"
[ ! -d "$LOGDIR" ] && mkdir "$LOGDIR"

# Validar argumento y definir nombre de proyecto
if [ $# -eq 0 ]; then
    echo "Please provide a folder name as a command line argument"
    exit 1
fi
folder_to_document="$1"
nombre_proyecto="$(basename "$folder_to_document")"

# Forzar el log a llamarse como el proyecto y guardarse en Logs/
LOGFILE="${LOGDIR}/${nombre_proyecto}.log"
exec > "$LOGFILE" 2>&1

if [ -f .env ]; then
    export $(grep -v '^#' .env | xargs)
fi

BITO_CMD=$(which bito)
BITO_CMD_VEP=""
BITO_VERSION=$($BITO_CMD -v | awk '{print $NF}')
if awk "BEGIN {exit !($BITO_VERSION > 3.7)}"; then
    BITO_CMD_VEP="--agent create_overview_doc"
fi

prompt_folder="AI_Prompts"
total_input_token_count=0
total_output_token_count=0
lang_csv="programming_languages.csv"
skip_list_csv="skip_list.csv"
progress_log=$(mktemp)
progress_lock="${progress_log}.lock"
trap 'rm -f "$progress_log" "$progress_lock"' EXIT

function bito_response_ok() {
    local ret_code=$1
    local response=$2
    if [[ $ret_code -ne 0 ]]; then return 1; fi
    if [[ $response == Whoops* ]]; then return 1; fi
    if [[ -z $response ]]; then return 1; fi
    return 0
}

function update_token_usage() {
    local input_tokens=$(echo "$1" | wc -w | awk '{print int($1 * 1.34)}')
    local output_tokens=$(echo "$2" | wc -w | awk '{print int($1 * 1.34)}')
    total_input_token_count=$((total_input_token_count + input_tokens))
    total_output_token_count=$((total_output_token_count + output_tokens))
}

function log_token_usage_and_session_duration() {
    local duration=$(( $(date +%s) - start_time ))
    echo "-----------------------------------------" | tee -a "$log_file"
    echo "$(date "+%Y-%m-%d %H:%M:%S") - Total Token Usage for Session" | tee -a "$log_file"
    echo "Total Input Tokens = $total_input_token_count" | tee -a "$log_file"
    echo "Total Output Tokens = $total_output_token_count" | tee -a "$log_file"
    echo "Session Duration: $((duration / 3600))h $(((duration % 3600) / 60))m $((duration % 60))s" | tee -a "$log_file"
    echo "-----------------------------------------" | tee -a "$log_file"
}

function check_tools_and_files() {
    echo "Checking required tools and files for documentation generation..." >&2
    local required_tools=("bito" "mmdc")
    local required_files=("high_level_doc_prompt.txt" "mermaid_doc_prompt.txt" "system_introduction_prompt.txt" "system_overview_mermaid_update_prompt.txt" "fix_mermaid_syntax_prompt.txt")
    for tool in "${required_tools[@]}"; do
        if ! command -v "$tool" &> /dev/null; then
            echo -e "\nError: Tool $tool is required but not found."
            exit 1
        fi
    done
    for file in "${required_files[@]}"; do
        if [ ! -f "$prompt_folder/$file" ]; then
            echo -e "\nError: Missing required file: $prompt_folder/$file"
            exit 1
        fi
    done
    echo -e "All required tools and files are present. Proceeding...\n" >&2
}

function read_skip_list() {
    if [ -f "$skip_list_csv" ]; then
        skip_list=()
        while IFS=, read -r skip_item; do
            skip_list+=("$skip_item")
        done < "$skip_list_csv"
    else
        echo "Skip list file $skip_list_csv not found."
        exit 1
    fi
}

function is_skippable() {
    local path=$1
    local skip_dirs_files=("logs" "node_modules" "dist" "target" "bin" "package-lock.json" "data.json" "build" ".gradle" ".idea" "gradle" "extension.js" "vendor.js" "ngsw.json" "polyfills.js" "init" ".gv")
    for skip_item in "${skip_dirs_files[@]}"; do
        if [[ "$path" == *"$skip_item"* ]]; then
            return 0
        fi
    done
    if [[ $(basename "$path") == .* ]]; then
        return 0
    fi
    return 1
}

function call_bito_with_retry() {
    local input_text=$1
    local prompt_file_path=$2
    local attempt=1
    local output
    local MAX_RETRIES=5
    local RETRY_DELAY=10
    local filename=$(basename "$prompt_file_path")
    while [ $attempt -le $MAX_RETRIES ]; do
        echo "Calling bito with retry logic. Attempt $attempt of $MAX_RETRIES with prompt file '$filename'..." >&2
        output=$(echo -e "$input_text" | bito $BITO_CMD_VEP -k "$BITO_API_KEY" -p "$prompt_file_path")
        local ret_code=$?
        if ! bito_response_ok "$ret_code" "$output"; then
            echo "Attempt $attempt: bito call for file '$filename' completed but returned insufficient content. Retrying in $RETRY_DELAY seconds..." >&2
            sleep $RETRY_DELAY
            ((attempt++))
        else
            echo "Attempt $attempt: Success! bito call for file '$filename' returned sufficient content." >&2
            echo "$output"
            update_token_usage "$input_text" "$output"
            return 0
        fi
    done
    echo "All $MAX_RETRIES attempts to call bito with prompt file '$filename' have failed to return adequate content. Exiting with error." >&2
    return 1
}

function create_module_documentation() {
    local path_to_module="$1"
    local documentation_directory="$2"
    echo "Creating documentation for module: $path_to_module" >&2

    if is_skippable "$path_to_module"; then
        echo "Skipped $path_to_module as it's on the exclusion list."
        return
    fi

    local name_of_module=$(basename "$path_to_module")
    local content_of_module=$(<"$path_to_module")

    local high_level_documentation
    high_level_documentation=$(call_bito_with_retry "Module: $name_of_module\n---\n$content_of_module" "$prompt_folder/high_level_doc_prompt.txt")
    local ret_code=$?
    if ! bito_response_ok "$ret_code" "$high_level_documentation"; then
        echo "High-level documentation creation failed for module: $name_of_module"
        return 1
    fi
    update_token_usage "$content_of_module" "$high_level_documentation"

    local mermaid_diagram=$(create_mermaid_diagram "$name_of_module" "$content_of_module")
    if [ $? -ne 0 ]; then
        echo "Mermaid diagram creation failed for module: $name_of_module"
        return 1
    fi

    local mdd_file="$documentation_directory/$name_of_module.mdd"
    if [ ! -s "$mdd_file" ]; then
        echo -e "$mermaid_diagram" > "$mdd_file"
    fi
    update_token_usage "$content_of_module" "$mermaid_diagram"

    local markdown_documentation_file="$documentation_directory/${name_of_module}_Doc.md"
    echo -e "## Module: $name_of_module\n$high_level_documentation" >> "$markdown_documentation_file"
    if [[ -n "$mermaid_diagram" && "$mermaid_diagram" =~ [^[:space:]] ]]; then
        echo -e "## Flow Diagram [via mermaid]\n\`\`\`mermaid\n$mermaid_diagram\n\`\`\`" >> "$markdown_documentation_file"
    fi

    echo -e "Documentation saved to $markdown_documentation_file\n\n"
}

function extract_module_names_and_associated_objectives_then_call_bito() {
    local filename=$1
    local prompt_file_path=$2
    local lines=()
    local current_module=""
    local current_objectives=""
    local capture_objectives=false
    local combined_output=""
    local attempt=1
    local MAX_RETRIES=5
    local RETRY_DELAY=10
    local bito_output

    echo -e "Extracting module names and objectives from file: $filename\n" >&2

    while IFS= read -r line; do
        lines+=("$line")
    done < "$filename"

    for line in "${lines[@]}"; do
        if [[ $line =~ ^##\ Module:\ (.*) ]]; then
            if [[ -n $current_module ]]; then
                echo "Processing module: $current_module with objectives" >&2
                combined_output+="Module: $current_module\n---\nPrimary Objectives:\n$current_objectives\n\n"
            fi
            current_module=${BASH_REMATCH[1]}
            current_objectives=""
            capture_objectives=false
        elif [[ $line =~ ^-\ \*\*Primary\ Objectives\*\*: ]]; then
            capture_objectives=true
            current_objectives+=$(echo $line | sed 's/.*\*\*Primary Objectives\*\*: //')$'\n'
        elif $capture_objectives && [[ $line =~ ^-\ .+ ]]; then
            current_objectives+=$(echo $line | sed 's/^-\ //')$'\n'
        elif [[ $line == "## "* || $line == "" ]]; then
            capture_objectives=false
        fi
    done

    if [[ -n $current_module ]]; then
        echo "Processing module: $current_module with objectives" >&2
        combined_output+="Module: $current_module\n---\nPrimary Objectives:\n$current_objectives\n\n"
    fi

    while [ $attempt -le $MAX_RETRIES ]; do
        echo "Attempt $attempt: Running bito for module: $current_module" >&2
        bito_output=$(echo -e "$combined_output" | bito $BITO_CMD_VEP -p "$prompt_file_path")
        local ret_code=$?
        if ! bito_response_ok "$ret_code" "$bito_output"; then
            echo "Attempt $attempt: bito command for module: $current_module failed or did not return enough content. Retrying in $RETRY_DELAY seconds..." >&2
            sleep $RETRY_DELAY
            ((attempt++))
        else
            echo "Attempt $attempt: bito command for module: $current_module succeeded with sufficient content." >&2
            echo "$bito_output"
            update_token_usage "$combined_output" "$bito_output"
            return 0
        fi
    done

    echo "Failed to call bito for module: $current_module after $MAX_RETRIES attempts with adequate content." >&2
    return 1
}

function fix_mermaid_syntax() {
    local mermaid_content="$1"
    local fixed_mermaid_content
    fixed_mermaid_content=$(echo "$mermaid_content" | sed 's/()//g; s/"//g')
    fixed_mermaid_content=$(echo "$fixed_mermaid_content" | sed 's/\[\//[ \//g')
    fixed_mermaid_content=$(echo "$fixed_mermaid_content" | sed 's/\[\([^]]*\)\[\([^]]*\)\]\]/[\1 \2]/g')
    echo "$fixed_mermaid_content"
}

function fix_mermaid_syntax_with_bito() {
    local fixed_mermaid_content
    fixed_mermaid_content=$(echo "$mermaid_content" | bito $BITO_CMD_VEP -p "$prompt_folder/mermaid_doc_prompt.txt" | awk '/^```mermaid$/,/^```$/{if (!/^```mermaid$/ && !/^```$/) print}')
    local ret_code=$?
    if ! bito_response_ok "$ret_code" "$fixed_mermaid_content"; then
        echo "Error in bito response for fixing mermaid syntax with bito."
        return 1
    fi
}

function validate_mermaid_syntax() {
    local mermaid_content="$1"
    local temp_mmd_file=$(mktemp)
    echo "$mermaid_content" > "$temp_mmd_file"
    local output=$(mmdc -i "$temp_mmd_file" -o /dev/null 2>&1)
    local status=$?
    rm "$temp_mmd_file"
    if [ $status -ne 0 ]; then
        echo "Mermaid syntax validation failed. Please check the diagram syntax." >&2
        echo "$output" >&2
        return 1
    fi
    return 0
}

function fix_and_validate_mermaid() {
    local mermaid_content="$1"
    local fixed_mermaid_content
    fixed_mermaid_content=$(fix_mermaid_syntax "$mermaid_content")
    if validate_mermaid_syntax "$fixed_mermaid_content"; then
        echo "Fixed Mermaid syntax is valid." >&2
        echo "$fixed_mermaid_content"
        return 0
    else
        echo "Fixed Mermaid syntax is invalid. Attempting to fix with bito..." >&2
        fixed_mermaid_content=$(fix_mermaid_syntax_with_bito "$fixed_mermaid_content")
        fixed_mermaid_content=$(fix_mermaid_syntax "$fixed_mermaid_content")
        if validate_mermaid_syntax "$fixed_mermaid_content"; then
            echo "Bito re-fixed Mermaid syntax and is valid." >&2
            echo "$fixed_mermaid_content"
            return 0
        else
            echo "Failed to fix Mermaid syntax even with bito and re-fixing." >&2
            return 1
        fi
    fi
}

function generate_mermaid_diagram() {
    local md_file="$1"
    local md_file_base="${md_file%.md}"
    if command -v mmdc >/dev/null 2>&1; then
        echo "Generating Mermaid diagrams in markdown file..."
        mmdc -i "${md_file_base}.md" -o "${md_file_base}.md" && echo "Mermaid diagrams updated in ${md_file_base}.md" || echo "Failed to update diagrams."
    else
        echo "Mermaid CLI not found; skipping diagram generation."
    fi
}

function create_mermaid_diagram() {
    local module_name="$1"
    local module_contents="$2"
    local mermaid_definition="flowchart\n$module_contents"
    local mermaid_flow_map
    local attempt=1
    local MAX_RETRIES=10
    local RETRY_DELAY=5
    local error_message=""
    local bito_output
    while [ $attempt -le $MAX_RETRIES ]; do
        echo "Attempt $attempt: Creating Mermaid diagram for module: $module_name" >&2
        bito_output=$(echo -e "Module: $module_name\n---\n$mermaid_definition" | bito $BITO_CMD_VEP -p "$prompt_folder/mermaid_doc_prompt.txt")
        local ret_code=$?
        if ! bito_response_ok "$ret_code" "$bito_output"; then
            echo "Attempt $attempt: Bito call failed or returned insufficient content. Retrying in $RETRY_DELAY seconds..." >&2
            sleep $RETRY_DELAY
            ((attempt++))
        else
            mermaid_flow_map=$(echo "$bito_output" | awk '/^```mermaid$/,/^```$/{if (!/^```mermaid$/ && !/^```$/) print}')
            mermaid_flow_map=$(fix_and_validate_mermaid "$mermaid_flow_map")
            local fix_and_validate_status=$?
            if [ $fix_and_validate_status -eq 0 ]; then
                echo "Mermaid diagram created successfully." >&2
                echo "$mermaid_flow_map"
                update_token_usage "$mermaid_definition" "$mermaid_flow_map"
                return 0
            else
                echo "Attempt $attempt: Failed to fix or validate Mermaid syntax. Retrying..." >&2
                sleep $RETRY_DELAY
                ((attempt++))
            fi
        fi
    done
    echo "Failed to create Mermaid diagram for module: $module_name after $MAX_RETRIES attempts."
    return 1
}

function generate_mdd_overview() {
    local dir="$1"
    local mermaid_doc_prompt_file="$prompt_folder/system_overview_mermaid_update_prompt.txt"
    local overview_mdd_file="$dir/overview.mdd"
    local temp_file
    local MAX_RETRIES=5
    local RETRY_DELAY=5
    echo "Starting to generate overview.mdd..."
    local latest_valid_mermaid_content=""
    for mdd_file in "$dir"/*.mdd; do
        if [ -f "$mdd_file" ] && [ "$mdd_file" != "$overview_mdd_file" ]; then
            echo "Processing $mdd_file..."
            local mermaid_script=$(cat "$mdd_file")
            mermaid_script=$(fix_and_validate_mermaid "$mermaid_script")
            local fix_status=$?
            if [ $fix_status -ne 0 ]; then
                echo "Mermaid diagram fixing/validation failed for $mdd_file. Skipping..."
                continue
            fi
            echo "Mermaid script found. Processing with bito..."
            local attempt=1
            while [ $attempt -le $MAX_RETRIES ]; do
                echo "Attempt $attempt: Processing Mermaid script for $mdd_file" >&2
                temp_file=$(mktemp)
                echo -e "$mermaid_script" | bito $BITO_CMD_VEP -p "$mermaid_doc_prompt_file" > "$temp_file"
                if validate_mermaid_syntax "$(cat "$temp_file")"; then
                    echo -e "Valid Mermaid diagram generated successfully for $mdd_file.\n" >&2
                    latest_valid_mermaid_content=$(cat "$temp_file")
                    rm "$temp_file"
                    rm "$mdd_file"
                    update_token_usage "$mermaid_script" "$latest_valid_mermaid_content"
                    break
                else
                    echo -e "Invalid Mermaid diagram syntax for attempt $attempt. Retrying...\n" >&2
                    rm "$temp_file"
                    sleep $RETRY_DELAY
                    ((attempt++))
                fi
            done
            if [ $attempt -gt $MAX_RETRIES ]; then
                echo "Failed to generate a valid Mermaid diagram for $mdd_file after $MAX_RETRIES attempts."
                return 1
            fi
        fi
    done
    if [ -n "$latest_valid_mermaid_content" ]; then
        echo -e "$latest_valid_mermaid_content" > "$overview_mdd_file"
        echo "Mermaid overview generated successfully: $overview_mdd_file"
    else
        echo "Failed to create overview.mdd or no valid diagrams were found."
        return 1
    fi
}

function create_find_command() {
    local lang_file="$1"
    local folder_to_document="$2"
    local find_command="find \"$folder_to_document\" -type f"
    while IFS= read -r ext; do
        find_command="$find_command -o -name \"*.$ext\""
    done < "$lang_file"
    find_command="${find_command/-o /\\( } \)"
    echo "$find_command"
}

function update_progress_time_log() {
    local processed=$1
    local total=$2
    local start_time=$3
    local file_name="${4:-N/A}"
    local char_count="${5:-N/A}"
    local file_time="${6:-N/A}"

    local file_end_time=$(date +%s)
    local file_start_time=$((file_end_time - file_time))
    local file_start_fmt=$(date -d "@$file_start_time" '+%d/%m/%Y:%H:%M:%S')
    local file_end_fmt=$(date -d "@$file_end_time" '+%d/%m/%Y:%H:%M:%S')
    local total_end_fmt=$(date '+%d/%m/%Y:%H:%M:%S')

    echo "Archivo: $file_name | Caracteres: $char_count | Tiempo inicio archivo: $file_start_fmt | Tiempo fin archivo: $file_end_fmt | Tiempo total fin: $total_end_fmt" >> "$progress_time_log"
}

MAX_JOBS=5

function parallel_create_module_documentation() {
    local files=("$@")
    local total_files=${#files[@]}
    local start_time=$(date +%s)
    for module_file in "${files[@]}"; do
        (
            local file_start_time=$(date +%s)
            create_module_documentation "$module_file" "$docs_folder"
            local file_end_time=$(date +%s)
            local file_time=$((file_end_time - file_start_time))
            local file_name=$(basename "$module_file")
            local char_count=$(wc -m < "$module_file")
            (
                flock 200
                processed_now=$(($(cat "$progress_log" 2>/dev/null || echo 0) + 1))
                echo "$processed_now" > "$progress_log"
                update_progress_time_log "$processed_now" "$total_files" "$start_time" "$file_name" "$char_count" "$file_time"
            ) 200>"$progress_lock"
        ) &
        while [[ $(jobs -rp | wc -l) -ge $MAX_JOBS ]]; do
            sleep 0.5
        done
    done
    wait
    echo "Log de progreso y tiempo guardado en: $progress_time_log"
}

start_time=$(date +%s)

function main() {
    check_tools_and_files
    docs_folder="doc_${nombre_proyecto}"
    [ ! -d "$docs_folder" ] && mkdir "$docs_folder"

    progress_time_log="Logs/${nombre_proyecto}_metricas.log"
    export progress_time_log
    log_file="Logs/bito_usage_log.txt"

    echo "Generating documentation in $docs_folder"
    echo "Documentation will be saved in $docs_folder" >&2
    echo "Progress log (metricas) will be saved in $progress_time_log" >&2
    echo "Bito token usage log will be saved in $log_file" >&2

    echo "0" > "$progress_log"
    > "$progress_time_log"

    read_skip_list
    aggregated_md_file="$docs_folder/High_Level_Doc.md"
    [ -f "$aggregated_md_file" ] && > "$aggregated_md_file"
    module_files_list=$(eval $(create_find_command "$lang_csv" "$folder_to_document"))
    readarray -t module_files <<<"$module_files_list"
    [ ${#module_files[@]} -eq 0 ] && echo "Warning: No files found for documentation generation." && return
    parallel_create_module_documentation "${module_files[@]}"
    echo "# Module Overview" > "$aggregated_md_file"
    for md_file in "$docs_folder"/*_Doc.md; do
        if [ "$md_file" != "$aggregated_md_file" ]; then
            cat "$md_file" >> "$aggregated_md_file"
        fi
    done
    local introduction_and_summary=$(extract_module_names_and_associated_objectives_then_call_bito "$aggregated_md_file" "$prompt_folder/system_introduction_prompt.txt")
    local temp_file=$(mktemp)
    mv "$aggregated_md_file" "$temp_file"
    echo -e "# Introduction :\n" > "$aggregated_md_file"
    echo "$introduction_and_summary" >> "$aggregated_md_file"
    generate_mdd_overview "$docs_folder" "$aggregated_md_file"
    if [ -f "$docs_folder/overview.mdd" ]; then
        echo -e "\n# Full System Overview\n" >> "$aggregated_md_file"
        awk '/^```mermaid$/,/^```$/' "$docs_folder/overview.mdd" >> "$aggregated_md_file"
    else+------------------
        echo "Overview file not found or empty."
    fi
    cat "$temp_file" >> "$aggregated_md_file"
    rm "$temp_file"
    generate_mermaid_diagram "$aggregated_md_file"
    log_token_usage_and_session_duration
    echo "Documentation generated in $docs_folder"
}
main "$@"