# StaticAnalyzerXML

Viewer for VS C++ static analyzer .xml files

MSVC compiler have the /analze option to run static analysis:

https://msdn.microsoft.com/en-us/library/ms173498.aspx

to save this report to a .xml file the option /analyze:log filename is provided. This program is a viewer for those .xml files. 

# How to create the XML files?

I'll use SQLite as example (sorry, nothing personal)

https://www.sqlite.org/howtocompile.html

Download the source files and run MSVC command prompt. Compile:

cl sqlite3.c /analyze:only /analyze /analyze:log analyzer_report.xml

You should have a file "analyzer_report.xml"

#How to use the program?

Download the program, open the solution, build & pray than run. If you are lucky and I've published a buildable version you will have a dialog application running. Appologies for the interace. 

In the upper corner there is a "browse to folder" edit box - select the folder containing the .xml file and the source. Than check if the default file prefix is correct - it will scan for .xml files that start with that prefix that folder recursively. This is made if you have multiple projects and the .xml files are usually created where tha make file is.

# GO!

If you continue to be lucky the .xml files will be found, parsed and the tree control will be populated with defects.

Than you should be able to browse and inspect them. I hope the UI and button names are self-explanatory.

#Viewing a warning in Microsoft Visual Studio

Yes! I have this feature.

Launch Visual Studio, expand any defect and double click on the file name or the description.

If such file at this path exists (if you send the .xml to someone else it wont) it will be oppened in Microsoft Visual Studio and the warning in question will be selected. This works almost always (the function is called visual_studio_open_file if you see it you will know why)!

You need MS Visual Studio to be running. 

#Thank you

And I hope you find this helpful. Please use whatever you find useful in the source - no need to ask. 
