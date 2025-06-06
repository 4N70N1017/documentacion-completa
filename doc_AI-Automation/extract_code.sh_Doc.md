## Module: extract_code.sh

# Documentación Técnica: extract_code.sh

## 1. **Nombre del módulo o componente SQL:**
extract_code.sh

## 2. **Objetivos principales:**
Este script de Bash está diseñado para extraer bloques de código de un archivo fuente (posiblemente un archivo Markdown) y guardarlos como archivos independientes según el lenguaje de programación especificado en el bloque. Su propósito es facilitar la extracción automática de ejemplos de código desde documentación técnica o tutoriales.

## 3. **Funciones, métodos o consultas críticas:**
- Lectura línea por línea del archivo fuente mediante un bucle `while`
- Detección de delimitadores de bloques de código (```lenguaje y ```)
- Extracción del contenido entre delimitadores
- Generación de nombres de archivo basados en el lenguaje detectado
- Escritura de archivos de salida con el código extraído

## 4. **Variables y elementos clave:**
- `filename`: Nombre del archivo fuente
- `filename_no_ext`: Nombre del archivo sin extensión
- `in_code_block`: Bandera booleana que indica si se está dentro de un bloque de código
- `block_content`: Almacena el contenido del bloque de código actual
- `block_lang`: Almacena el lenguaje del bloque de código actual
- `*_counter`: Contadores para cada tipo de lenguaje (python, javascript, bash, default)
- `output_file`: Nombre del archivo de salida generado

## 5. **Interdependencias y relaciones:**
El script depende de:
- Comandos estándar de Bash como `basename`, `read`, y operadores de manipulación de cadenas
- Un archivo fuente válido proporcionado como argumento
- Permisos de escritura en el directorio actual para crear los archivos de salida

## 6. **Operaciones centrales vs. auxiliares:**
- **Centrales**: Detección de bloques de código, extracción del contenido, determinación del lenguaje, escritura de archivos
- **Auxiliares**: Validación de argumentos, extracción del nombre de archivo, conteo de bloques por lenguaje

## 7. **Secuencia operativa o flujo de ejecución:**
1. Validación de argumentos de entrada
2. Extracción del nombre del archivo sin extensión
3. Inicialización de variables
4. Lectura línea por línea del archivo fuente
5. Detección de inicio/fin de bloques de código
6. Extracción del lenguaje y contenido del bloque
7. Generación de nombre de archivo basado en el lenguaje
8. Escritura del contenido en el archivo de salida
9. Repetición hasta procesar todo el archivo fuente

## 8. **Aspectos de rendimiento y optimización:**
- El script procesa el archivo línea por línea, lo que es eficiente para archivos de tamaño moderado
- No carga todo el archivo en memoria, lo que permite manejar archivos grandes
- La detección de bloques de código es simple y directa, sin operaciones costosas

## 9. **Reusabilidad y adaptabilidad:**
- El script es altamente reutilizable para extraer código de cualquier archivo con formato Markdown
- Se puede adaptar fácilmente para soportar más lenguajes modificando la estructura case
- Podría extenderse para manejar directorios completos o para incluir opciones adicionales

## 10. **Uso y contexto:**
Se utiliza ejecutándolo desde la línea de comandos con un archivo fuente como argumento:
```
./extract_code.sh archivo_fuente.md
```
Es útil en flujos de trabajo de documentación técnica, preparación de materiales de enseñanza, o extracción de ejemplos de código para pruebas.

## 11. **Supuestos y limitaciones:**
- Supone que los bloques de código están correctamente delimitados con ``` al inicio y al final
- Asume que el lenguaje se especifica inmediatamente después del primer delimitador
- No maneja bloques de código anidados
- No preserva indentación original en algunos casos
- Limitado a los lenguajes explícitamente definidos (python, javascript/js, bash)
- No valida la sintaxis del código extraído
## Flow Diagram [via mermaid]
```mermaid
Failed to create Mermaid diagram for module: extract_code.sh after 10 attempts.
```
