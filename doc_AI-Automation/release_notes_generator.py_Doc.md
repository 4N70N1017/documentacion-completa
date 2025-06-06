## Module: release_notes_generator.py

# Documentación Técnica: release_notes_generator.py

## 1. **Nombre del módulo o componente SQL:**
release_notes_generator.py

## 2. **Objetivos principales:**
Este script automatiza la generación de notas de lanzamiento (release notes) a partir de las diferencias entre versiones de código en un repositorio Git. Su propósito es facilitar la documentación de cambios entre versiones, ahorrando tiempo al desarrollador y proporcionando un formato consistente para las notas de lanzamiento.

## 3. **Funciones, métodos o consultas críticas:**
- `generate_release_notes()`: Función principal que genera las notas de lanzamiento a partir de diferencias en el código.
- `parse_arguments()`: Procesa los argumentos de línea de comandos para configurar la ejecución.
- `main()`: Punto de entrada que coordina la ejecución del script.

## 4. **Variables y elementos clave:**
- `repo_path`: Ruta al repositorio Git.
- `prompt_path`: Ruta al archivo de prompt utilizado por la herramienta Bito.
- `commit1` y `commit2`: Referencias a commits para comparar (tags o hashes).
- `diff_file`: Archivo que contiene las diferencias entre versiones.
- `diff_command`: Comando Git para generar las diferencias.
- `bito_command`: Comando para procesar las diferencias y generar las notas.

## 5. **Interdependencias y relaciones:**
- Depende de Git para obtener las diferencias entre versiones.
- Utiliza la herramienta CLI de Bito para procesar las diferencias y generar las notas.
- Interactúa con el sistema de archivos para leer/escribir archivos temporales y de salida.

## 6. **Operaciones centrales vs. auxiliares:**
- **Centrales**: 
  - Generación de diferencias Git entre versiones.
  - Procesamiento de diferencias mediante Bito para crear notas de lanzamiento.
- **Auxiliares**:
  - Manejo de argumentos de línea de comandos.
  - Creación y eliminación de archivos temporales.
  - Registro de información en modo verbose.

## 7. **Secuencia operativa o flujo de ejecución:**
1. Parseo de argumentos de línea de comandos.
2. Determinación del comando Git adecuado según los parámetros proporcionados.
3. Ejecución del comando Git para obtener las diferencias.
4. Almacenamiento temporal de las diferencias en un archivo.
5. Procesamiento de las diferencias mediante Bito para generar las notas.
6. Escritura de las notas de lanzamiento en un archivo markdown.
7. Limpieza de archivos temporales.

## 8. **Aspectos de rendimiento y optimización:**
- El script utiliza subprocesos para ejecutar comandos externos, lo que puede ser un cuello de botella para repositorios grandes.
- No hay manejo específico para repositorios muy grandes donde el diff podría ser extenso.
- La ejecución es secuencial, lo que simplifica el código pero podría limitar el rendimiento.

## 9. **Reusabilidad y adaptabilidad:**
- El código es altamente parametrizable a través de argumentos de línea de comandos.
- La función principal `generate_release_notes()` puede ser importada y utilizada en otros scripts.
- El formato de las notas de lanzamiento depende del archivo de prompt, lo que permite personalización.

## 10. **Uso y contexto:**
- Se utiliza como herramienta de línea de comandos para automatizar la generación de documentación.
- Típicamente se ejecutaría durante el proceso de lanzamiento de una nueva versión de software.
- Ejemplo de uso: `python release_notes_generator.py --repo /path/to/repo --prompt /path/to/prompt --oldver v1.0 --newver v2.0`

## 11. **Supuestos y limitaciones:**
- Supone que Git está instalado y disponible en el sistema.
- Requiere la herramienta Bito CLI instalada y configurada.
- No maneja errores de Git o Bito de manera robusta.
- Asume que el repositorio es válido y accesible.
- Las notas generadas dependen completamente de la calidad del prompt proporcionado a Bito.
- No tiene capacidad para editar o refinar las notas generadas automáticamente.
## Flow Diagram [via mermaid]
```mermaid
Failed to create Mermaid diagram for module: release_notes_generator.py after 10 attempts.
```
