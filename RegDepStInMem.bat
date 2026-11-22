cd src\jre\lib
copy /y /b modules.001+modules.002+modules.003 modules
cd ..\bin
start java -jar EduSampleBoot-1.jar
timeout /t 3
start http://localhost/Ex9_pr3
start ..\guide.html
