cd src\jre\lib
copy /y /b modules.001+modules.002+modules.003 modules
cd ..\bin
start java -jar EduSampleBootJPA-1.jar
timeout /t 3
start http://localhost/Ex10
start ..\guide_Ex10.html
