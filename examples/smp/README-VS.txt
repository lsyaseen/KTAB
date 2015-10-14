---------------------------------------------
 Copyright KAPSARC. Open Source MIT License.
---------------------------------------------

In Visual Studio 2013 Express, you have to reset the Project -> Properties -> Linker -> 
System -> SubSystem properly in order to suppress the console;
see the attached screenshot "2015-01-30 setting console properties"


The cmake-gui tool builds a project file for a 
'Console' application, not a 'Windows' application.
Thus, it creates a black terminal window whenever it runs.
To fix this in MS Studio 2008, you can select the executable target,
right click to Properties->Linker->System->SubSystem, and
change it from Console to Application.

In MS Studio 2010 Professional, the process of suppressing the
console window for a project has two steps:
  Properties -> Linker -> System -> SubSystem: Windows(/SUBSYSTEM:WINDOWS)
  Properties -> Linker -> Advanced -> Entry Point: mainCRTStartup
or in the project build linker options set
  /SUBSYSTEM:windows
  /ENTRY:mainCRTStartup

See this posting for more details:
http://stackoverflow.com/questions/2139637/hide-console-of-windows-application/6882500#6882500
--------------------------------------------
Another nagging problem has been the inability to attach an icon to a program.
I finally found ways to do it in MS Studio 2010 Professional.

The procedure is (roughly) to open the View -> Resource View of the project
(similar to the Project View),
right-click on the project and follow Add -> Resource -> Icon.
I then pointed it to the existing ico files in the top-level directory, and a little more
fiddling got it attached. Note that the foo.rc files need to be in 'build':
you need to re-build them there, so these are only for reference.
---------------------------------------------
 Copyright KAPSARC. Open Source MIT License.
---------------------------------------------

