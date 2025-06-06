## Module: recovery.py

# Análisis Integral del Módulo recovery.py

## Nombre del Módulo/Componente SQL
**recovery.py** - Módulo de procesamiento de informes de seguridad de Checkmarx

## Objetivos Primarios
Este módulo está diseñado para extraer, procesar y analizar información de vulnerabilidades de seguridad desde archivos PDF generados por la herramienta Checkmarx. Su propósito principal es transformar estos informes en formatos estructurados (CSV) para facilitar su análisis posterior, incluyendo la categorización de vulnerabilidades por severidad y la asociación con archivos específicos del proyecto.

## Funciones, Métodos y Consultas Críticas
1. **extraer_texto_de_pdf(ruta_pdf)**: Extrae el contenido textual de un archivo PDF.
2. **split_by_frags(txt)**: Divide el texto extraído en fragmentos basados en patrones específicos.
3. **get_info_frags(frags)**: Procesa los fragmentos para extraer información estructurada sobre vulnerabilidades.
4. **clean_info(info)**: Limpia y normaliza la información extraída.
5. **save_to_csv(info, base_path, file_name)**: Guarda la información procesada en un archivo CSV.
6. **sev_save_to_csv(info, base_path, file_name)**: Genera un resumen de vulnerabilidades por nivel de severidad.
7. **group_by_file_name(info)**: Agrupa las vulnerabilidades por nombre de archivo.
8. **actualizar_csv_con_tamano(base_path, csv_path)**: Actualiza el CSV con información sobre el tamaño de los archivos.

## Variables y Elementos Clave
1. **Expresiones regulares (RGX_*)**: Patrones para extraer información específica del texto.
2. **info_all_frags**: Almacena la información estructurada de todas las vulnerabilidades.
3. **base_path**: Ruta base donde se almacenan los archivos del proyecto.
4. **Parámetros de entrada**: nombre_aplicacion, nombre_pdf, id_proyecto.
5. **DELIMITER**: Delimitador utilizado en los archivos CSV ('|').

## Interdependencias y Relaciones
1. **Sistema de archivos**: Interactúa con el sistema de archivos para leer PDFs y escribir CSVs.
2. **Checkmarx**: Depende de la estructura específica de los informes generados por esta herramienta.
3. **Estructura de proyectos**: Asume una estructura específica de directorios (/sysx/bito/projects/).
4. **Bibliotecas externas**: Depende de PyPDF2 para la extracción de texto de PDFs.

## Operaciones Core vs. Auxiliares
**Core**:
- Extracción de texto de PDFs
- Análisis y estructuración de datos de vulnerabilidades
- Generación de informes CSV

**Auxiliares**:
- Búsqueda y medición de tamaño de archivos
- Validación de datos
- Manejo de errores y excepciones
- Limpieza y normalización de texto

## Secuencia Operacional/Flujo de Ejecución
1. Recibe parámetros de entrada (nombre de aplicación, PDF y ID de proyecto)
2. Localiza y extrae texto del PDF más reciente
3. Identifica la sección "Scan Results Details"
4. Divide el texto en fragmentos por vulnerabilidad
5. Extrae información estructurada de cada fragmento
6. Limpia y normaliza los datos
7. Agrupa vulnerabilidades por archivo
8. Genera un CSV detallado con todas las vulnerabilidades
9. Genera un CSV resumen con conteos por severidad
10. Actualiza el CSV con información de tamaño de archivos

## Aspectos de Rendimiento y Optimización
1. **Manejo de archivos grandes**: Aumenta el límite de tamaño de campo CSV para manejar grandes volúmenes de datos.
2. **Procesamiento secuencial**: El procesamiento línea por línea podría ser ineficiente para PDFs muy grandes.
3. **Búsqueda recursiva de archivos**: La función buscar_archivo podría ser costosa en proyectos con muchos archivos.
4. **Compilación previa de expresiones regulares**: Optimiza el rendimiento al compilar patrones regex antes de su uso.

## Reusabilidad y Adaptabilidad
1. **Modularidad**: Funciones bien definidas que pueden reutilizarse en otros contextos.
2. **Parametrización**: Acepta parámetros externos para adaptarse a diferentes proyectos.
3. **Limitaciones de adaptabilidad**: Fuertemente acoplado al formato específico de los informes de Checkmarx.
4. **Dependencia estructural**: Asume una estructura de directorios específica que limita su portabilidad.

## Uso y Contexto
Este módulo se utiliza en un entorno de desarrollo o seguridad para procesar automáticamente los resultados de análisis de seguridad de código. Se ejecuta como un script de línea de comandos que recibe tres parámetros: nombre de la aplicación, nombre del archivo PDF y ID del proyecto. Es probable que forme parte de un pipeline de integración continua o un proceso de revisión de seguridad.

## Suposiciones y Limitaciones
1. **Formato de PDF**: Asume una estructura específica en los informes de Checkmarx.
2. **Estructura de directorios**: Espera una estructura específica (/sysx/bito/projects/).
3. **Formato de nombres**: Asume convenciones específicas para nombres de archivos y proyectos.
4. **Limitaciones de procesamiento**: El análisis basado en expresiones regulares puede fallar si el formato del informe cambia.
5. **Manejo de caracteres especiales**: Puede haber problemas con caracteres especiales en los nombres de archivo o descripciones.
6. **Dependencia de bibliotecas**: Requiere PyPDF2 y otras bibliotecas estándar de Python.
## Flow Diagram [via mermaid]
```mermaid
Failed to create Mermaid diagram for module: recovery.py after 10 attempts.
```
