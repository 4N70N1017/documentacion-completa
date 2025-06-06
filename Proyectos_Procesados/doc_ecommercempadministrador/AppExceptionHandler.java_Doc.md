## Module: AppExceptionHandler.java

# Análisis Integral del Módulo AppExceptionHandler

## Nombre del Módulo/Componente SQL
**AppExceptionHandler.java** - Manejador de excepciones para una aplicación Spring Boot

## Objetivos Primarios
Este módulo tiene como propósito principal gestionar las excepciones no controladas que ocurren en la aplicación, proporcionando respuestas HTTP estructuradas y consistentes. Actúa como un interceptor global de excepciones, transformando los errores en respuestas API estandarizadas para mantener una experiencia de usuario coherente y facilitar la depuración.

## Funciones, Métodos y Consultas Críticas
1. **handleResponseStatusException**: Maneja específicamente las excepciones de tipo `ResponseStatusException`, generando respuestas con el código de estado y mensaje correspondientes.
2. **handleException**: Método genérico que captura cualquier excepción no manejada por otros métodos, devolviendo un error 500 (Internal Server Error) con información detallada.

## Variables y Elementos Clave
- **Meta**: Objeto que encapsula metadatos de respuesta (estado, código, mensaje, timestamp)
- **ApiResponseDto**: DTO que estructura la respuesta API con metadatos y datos (nulos en caso de error)
- **HttpHeaders**: Configuración de cabeceras HTTP para las respuestas
- **HttpStatus**: Códigos de estado HTTP utilizados para las respuestas

## Interdependencias y Relaciones
- Extiende **ResponseEntityExceptionHandler** de Spring Framework
- Utiliza **AppMessages** para mensajes estandarizados
- Depende de **ApiResponseDto** y **Meta** para estructurar las respuestas
- Se integra con el sistema de manejo de excepciones de Spring mediante la anotación `@ControllerAdvice`

## Operaciones Core vs. Auxiliares
- **Core**: Interceptación y transformación de excepciones en respuestas API estructuradas
- **Auxiliares**: Configuración de cabeceras HTTP, formateo de mensajes de error, registro de timestamps

## Secuencia Operacional/Flujo de Ejecución
1. Se produce una excepción en algún controlador de la aplicación
2. Spring detecta la excepción y la dirige al manejador apropiado:
   - Si es `ResponseStatusException`, se procesa en `handleResponseStatusException`
   - Para cualquier otra excepción, se procesa en `handleException`
3. Se crea un objeto `Meta` con la información del error
4. Se construye un `ApiResponseDto` con los metadatos y sin datos
5. Se configura la respuesta HTTP con las cabeceras adecuadas
6. Se devuelve la respuesta al cliente

## Aspectos de Rendimiento y Optimización
- El manejo centralizado de excepciones evita código duplicado en los controladores
- La estructura de respuesta estandarizada facilita el procesamiento por parte de los clientes
- No hay operaciones intensivas que puedan generar cuellos de botella

## Reusabilidad y Adaptabilidad
- Alta reusabilidad gracias a la anotación `@ControllerAdvice` que aplica a toda la aplicación
- Fácilmente adaptable para manejar tipos específicos de excepciones adicionales
- La estructura de respuesta estandarizada facilita la integración con diferentes clientes

## Uso y Contexto
- Se utiliza automáticamente en toda la aplicación para capturar excepciones no manejadas
- Proporciona un mecanismo uniforme para comunicar errores a los clientes de la API
- Forma parte de la capa de presentación/API de la aplicación

## Suposiciones y Limitaciones
- **Suposiciones**:
  - Se asume que todas las respuestas de error deben seguir el formato definido por `ApiResponseDto`
  - Se espera que los mensajes de error estén definidos en `AppMessages`
  
- **Limitaciones**:
  - No maneja excepciones específicas del negocio (solo `ResponseStatusException` y genéricas)
  - No implementa mecanismos de registro (logging) detallados para las excepciones
  - No proporciona mecanismos para recuperación automática de errores
## Flow Diagram [via mermaid]
```mermaid
Failed to create Mermaid diagram for module: AppExceptionHandler.java after 10 attempts.
```
