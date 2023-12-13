# proyectoIngBarraza
INTÉRPRETE DE COMANDOS CON C/C++
EVALUACIÓN DE PRODUCTO: “INTÉRPRETE DE COMANDOS CON C/C++”

INDICACIONES PARA EL DESARROLLO
Desarrolle un intérprete de comandos simplificado. La función del intérprete será pedir un comando o
programa, ejecutarlo y volver a pedir otro comando.

El intérprete debe debe tener las siguientes características:
  ● El intérprete debe utilizar un prompt personalizado.
  ● Los comandos deben ejecutarse escribiendo su ruta absoluta. En caso de no escribir la ruta
    absoluta, se debe suponer que se encuentra en el directorio /bin.
  ● La invocación a un comando o programa, debe realizarse por medio de un fork y la ejecución de
    dicho programa, todo ello con un proceso hijo del intérprete de comandos.
  ● Si el comando ingresado es incorrecto, deberá visualizar un mensaje de error.
  ● Al ejecutar un comando o programa (proceso hijo), el intérprete de comandos (proceso padre)
    debe quedarse en espera (función wait).
  ● El intérprete debe soportar el redireccionamiento de la salida. Para la redirección el intérprete
    deberá reconocer el carácter “>”. Si el usuario ingresa nombreprograma arg2 > texto, se deberá
    crear el fichero texto donde se volcará todo el contenido de la salida de nombreprograma, por lo
    que este no deberá mostrarse en pantalla.
  ● El intérprete deberá cerrarse cuando se ingrese la palabra salir.
  
Nota: Se debe asumir que todos los argumentos de un comando (incluyendo el símbolo de redirección >)
estarán separados por espacios en blanco.

Cualquier otra característica que se añada al trabajo (tuberías, trabajo en segundo plano, redirección de
entrada “<”, uso de makefiles) tendrá nota adicional.

RESPECTO A LA PRESENTACIÓN DEL TRABAJO:
  ● El trabajo se desarrollará en forma individual.
  ● Deberá programar el código fuente usando C o C++ bajo cualquier entorno Linux, deberá utilizar los
    comandos aprendidos en las sesiones de práctica (librerías del estándar POSIX). El código fuente
    deberá estar correctamente /*comentado*/, de tal forma que sea fácil entender para sus colegas
    y evitar innecesariamente una gimnasia mental.
  ● Deberá realizar el control de versionamiento usando Git
