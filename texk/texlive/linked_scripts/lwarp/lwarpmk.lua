#!/usr/bin/env texlua

-- Copyright 2016 Brian Dunn

-- Print the usage of the lwarpmk command:

printversion = "v0.20"

function printhelp ()
print ("lwarpmk: Use lwarpmk -h or lwarpmk --help for help.") ;
end

function printusage ()
print ( [[

lwarpmk print [project]: Compile a print version.
lwarpmk printindex [project]: Process the index for the print version.
lwarpmk html [project]: Compile an HTML version.
lwarpmk htmlindex [project]: Process the index for the html version.
lwarpmk again [project]: Touch the source code to trigger recompiles.
lwarpmk limages [project]: Process the "lateximages" created by lwarp.sty.
lwarpmk pdftohtml [project]:
    For use with latexmk or a Makefile:
    Convert project_html.pdf to project_html.html and
    individual HTML files.
lwarpmk clean [project]: Remove project.aux, .toc, .lof, .lot, .idx, .ind, .log
lwarpmk cleanall [project]: Remove auxiliary files and also project.pdf, *.html
lwarpmk -h: Print this help message.
lwarpmk --help: Print this help message.

]] )
printconf ()
end

-- Print the format of the configuration file lwarpmk.conf:

function printconf ()
print ( [[
An example lwarpmk.conf or <project>.lwarpmkconf project file:
--
opsystem = "Unix"   (or "Windows")
latexname = "pdflatex"  (or "lualatex", or "xelatex")
sourcename = "projectname"  (the source-code filename w/o .tex)
homehtmlfilename = "index"  (or perhaps the project name)
htmlfilename = ""  (or "projectname" - filename prefix)
uselatexmk = "false"  (or "true" to use latexmk to build PDFs)
--
Filenames must contain only letters, numbers, underscore, or dash.
Values must be in "quotes".

]] ) ;
end

-- Split one large sourcefile into a number of files,
-- starting with destfile.
-- The file is split at each occurance of <!--|Start file|newfilename|*

function splitfile (destfile,sourcefile)
print ("lwarpmk: Splitting " .. sourcefile .. " into " .. destfile) ;
io.input(sourcefile)
io.output(destfile)
for line in io.lines() do
i,j,copen,cstart,newfilename = string.find (line,"(.*)|(.*)|(.*)|") ;
if ( (i~= nil) and (copen == "<!--") and (cstart == "Start file")) then -- split the file
io.output(newfilename) ;
else -- not a splitpoint
io.write (line .. "\n") ;
end
end -- do
end -- function

-- Incorrect value, so print an error and exit.

function cvalueerror ( line, linenum , cvalue )
    print ( linenum .. " : " .. line ) ;
    print ("lwarpmk: incorrect variable value \"" .. cvalue .. "\" in lwarpmk.conf.\n" ) ;
    printconf () ;
    os.exit(1) ;
end

-- Load settings from the project's "lwarpmk.conf" file:

function loadconf ()
-- Default configuration filename:
local conffile = "lwarpmk.conf"
-- Optional configuration filename:
if arg[2] ~= nil then conffile = arg[2]..".lwarpmkconf" end
-- Verify the file exists:
if (lfs.attributes(conffile,"mode")==nil) then -- file not exists
print("lwarpmk: " .. conffile .." does not exist.")
print("lwarpmk: " .. arg[2] .. " does not appear to be a project name.\n")
printhelp () ;
os.exit(1) -- exit the entire lwarpmk script
else -- file exists
-- Read the file:
print ("lwarpmk: Reading " .. conffile ..".")
io.input(conffile) ;
-- Scan each line:
local linenum = 0
for line in io.lines() do -- scan lines
linenum = linenum + 1
i,j,cvarname,cvalue = string.find (line,"([%w-_]*)%s*=%s*\"([%w-_]*)\"") ;
-- Error if incorrect enclosing characters:
if ( i == nil ) then
print ( linenum .. " : " .. line ) ;
print ( "lwarpmk: Incorrect entry in " .. conffile ..".\n" ) ;
printconf () ;
os.exit(1) ;
end
if ( cvarname == "opsystem" ) then
    -- Verify choice of opsystem:
    if ( (cvalue == "Unix") or (cvalue == "Windows") ) then
        opsystem = cvalue
    else
        cvalueerror ( line, linenum , cvalue )
    end
elseif ( cvarname == "latexname" ) then
    -- Verify choice of LaTeX compiler:
    if (
        (cvalue == "pdflatex") or
        (cvalue == "xelatex") or
        (cvalue == "lualatex")
    ) then
        latexname = cvalue
    else
        cvalueerror ( line, linenum , cvalue )
    end
elseif ( cvarname == "sourcename" ) then sourcename = cvalue
elseif ( cvarname == "homehtmlfilename" ) then homehtmlfilename = cvalue
elseif ( cvarname == "htmlfilename" ) then htmlfilename = cvalue
elseif ( cvarname == "uselatexmk" ) then uselatexmk = cvalue
else
print ( linenum .. " : " .. line ) ;
print ("lwarpmk: Incorrect variable name \"" .. cvarname .. "\" in " .. conffile ..".\n" ) ;
printconf () ;
os.exit(1) ;
end
end -- do scan lines
end -- file exists
-- Select some operating-system commands:
if opsystem=="Unix" then  -- For Unix / Linux / Mac OS:
rmname = "rm"
touchname = "touch"
chmodcmd = "chmod u+x lateximages.sh"
lateximagesname = "./lateximages.sh"
elseif opsystem=="Windows" then -- For Windows
rmname = "DEL"
touchname = "TOUCH"
chmodcmd = ""
lateximagesname = "lateximages.cmd"
else print ( "lwarpmk: Select Unix or Windows for opsystem" )
end --- for Windows
end -- loadconf

-- Scan the LaTeX log file for the phrase "Rerun to get",
-- indicating that the file should be compiled again.
-- Return true if found.

function reruntoget (filesource)
io.input(filesource)
for line in io.lines() do
if ( string.find(line,"Rerun to get") ~= nil ) then return true end
end
return false
end

-- Compile one time, return true if should compile again.
-- fsuffix is "" for print, "_html" for HTML output.

function onetime (fsuffix)
print("lwarpmk: Compiling with " .. latexname .. " " .. sourcename..fsuffix)
err = os.execute(
--    "echo " ..
    latexname .. " " .. sourcename..fsuffix )
if ( err ~= 0 ) then print ( "lwarpmk: Compile error.") ; os.exit(1) ; end
return (reruntoget(sourcename .. fsuffix .. ".log") ) ;
end

-- Compile up to five times.
-- fsuffix is "" for print, "_html" for HTML output

function manytimes (fsuffix)
if onetime(fsuffix) == true then
if onetime(fsuffix) == true then
if onetime(fsuffix) == true then
if onetime(fsuffix) == true then
if onetime(fsuffix) == true then
end end end end end
end

-- Exit if the given file does not exist.

function verifyfileexists (filename)
if (lfs.attributes ( filename , "modification" ) == nil ) then
print ( "lwarpmk: " .. filename .. " not found." ) ;
os.exit (1) ;
end
end

-- Convert <project>_html.pdf into HTML files:

function pdftohtml ()
    -- Convert to text:
    print ("lwarpmk: Converting " .. sourcename .."_html.pdf to " .. sourcename .. "_html.html")
    os.execute("pdftotext  -enc UTF-8  -nopgbrk  -layout " .. sourcename .. "_html.pdf " .. sourcename .. "_html.html")
    -- Split the result into individual HTML files:
    splitfile (homehtmlfilename .. ".html" , sourcename .. "_html.html")
end

-- Remove auxiliary files:

function removeaux ()
    os.execute ( rmname .. " " ..
        sourcename ..".aux " .. sourcename .. "_html.aux " ..
        sourcename ..".toc " .. sourcename .. "_html.toc " ..
        sourcename ..".lof " .. sourcename .. "_html.lof " ..
        sourcename ..".lot " .. sourcename .. "_html.lot " ..
        sourcename ..".idx " .. sourcename .. "_html.idx " ..
        sourcename ..".ind " .. sourcename .. "_html.ind " ..
        sourcename ..".log " .. sourcename .. "_html.log "
        )
end

-- lwarpmk --version :

if (arg[1] == "--version") then
print ( "lwarpmk: " .. printversion )

else -- not -- version

-- print intro:

print ("lwarpmk: " .. printversion .. "  Automated make for the LaTeX lwarp package.")

-- lwarpmk print:

if arg[1] == "print" then
loadconf ()
if ( uselatexmk == "true" ) then
    os.execute ( "latexmk -pdf -dvi- -ps- -pdflatex=\"" .. latexname .." %O %S\" " .. sourcename ..".tex" ) ;
    print ("lwarpmk: Done.")
else -- not latexmk
    verifyfileexists (sourcename .. ".tex") ;
    -- See if up to date:
    if (
        ( lfs.attributes ( sourcename .. ".pdf" , "modification" ) == nil ) or
        (
            lfs.attributes ( sourcename .. ".tex" , "modification" ) >
            lfs.attributes ( sourcename .. ".pdf" , "modification" )
        )
    ) then
        -- Recompile if not yet up to date:
        manytimes("")
        print ("lwarpmk: Done.") ;
    else
        print ("lwarpmk: " .. sourcename .. ".pdf is up to date.") ;
    end
end -- not latexmk

-- lwarp printindex:
-- Compile the index then touch the source
-- to trigger a recompile of the document:

elseif arg[1] == "printindex" then
loadconf ()
print ("lwarpmk: Processing the index.")
os.execute("texindy -M lwarp_html.xdy " .. sourcename .. ".idx")
print ("lwarpmk: Forcing an update of " .. sourcename ..".tex.")
os.execute(touchname .. " " .. sourcename .. ".tex")
print ("lwarpmk: " .. sourcename ..".tex is ready to be recompiled.")
print ("lwarpmk: Done.")

-- lwarpmk html:

elseif arg[1] == "html" then
loadconf ()
if ( uselatexmk == "true" ) then
    -- The recorder option is required to detect changes in <project>.tex
    -- while we are loading <project>_html.tex.
    err=os.execute ( "latexmk -pdf -dvi- -ps- -recorder "
        .. "-e '$makeindex = q/texindy -M lwarp_html.xdy/' "
        .. "-pdflatex=\"" .. latexname .." %O %S\" "
        .. sourcename .."_html.tex" ) ;
    if ( err ~= 0 ) then print ( "lwarpmk: Compile error.") ; os.exit(1) ; end
    pdftohtml ()
    print ("lwarpmk: Done.")
else -- not latexmk
    verifyfileexists ( sourcename .. ".tex" ) ;
    -- See if exists and is up to date:
    if (
        ( lfs.attributes ( homehtmlfilename .. ".html" , "modification" ) == nil ) or
        (
            lfs.attributes ( sourcename .. ".tex" , "modification" ) >
            lfs.attributes ( homehtmlfilename .. ".html" , "modification" )
        )
    ) then
        -- Recompile if not yet up to date:
        manytimes("_html")
        pdftohtml ()
        print ("lwarpmk: Done.")
    else
        print ("lwarpmk: " .. homehtmlfilename .. ".html is up to date.")
    end
end -- not latexmk

elseif arg[1] == "pdftohtml" then
    loadconf ()
    pdftohtml ()

-- lwarpmk htmlindex:
-- Compile the index then touch the source
-- to trigger a recompile of the document:

elseif arg[1] == "htmlindex" then
loadconf ()
print ("lwarpmk: Processing the index.")
os.execute("texindy -M lwarp_html.xdy " .. sourcename .. "_html.idx")
print ("lwarpmk: Forcing an update of " .. sourcename ..".tex.")
os.execute(touchname .. " " .. sourcename .. ".tex")
print ("lwarpmk: " .. sourcename ..".tex is ready to be recompiled.")
print ("lwarpmk: Done.")

-- lwarpmk limages:
-- Make the lateximages command file executable,
-- execute it to create the images,
-- then touch the source to trigger a recompile.

elseif arg[1] == "limages" then
loadconf ()
print ("lwarpmk: Processing images.")
os.execute(chmodcmd)
os.execute(lateximagesname)
print ("lwarpmk: Forcing an update of " .. sourcename ..".tex.")
os.execute(touchname .. " " .. sourcename .. ".tex") ;
print ("lwarpmk: " .. sourcename ..".tex is ready to be recompiled.")
print ("lwarpmk: Done.")

-- lwarpmk again:
-- Touch the source to trigger a recompile.

elseif arg[1] == "again" then
loadconf ()
print ("lwarpmk: Forcing an update of " .. sourcename ..".tex.")
os.execute(touchname .. " " .. sourcename .. ".tex") ;
print ("lwarpmk: " .. sourcename ..".tex is ready to be recompiled.")
print ("lwarpmk: Done.")

-- lwarpmk clean:
-- Remove project.aux, .toc, .lof, .lot, .idx, .ind, .log

elseif arg[1] == "clean" then
loadconf ()
removeaux ()
print ("lwarpmk: Done.")

-- lwarpmk cleanall
-- Remove project.aux, .toc, .lof, .lot, .idx, .ind, .log
--    and also project.pdf, *.html

elseif arg[1] == "cleanall" then
loadconf ()
removeaux ()
os.execute ( rmname .. " " ..
    sourcename .. ".pdf " .. sourcename .. "_html.pdf " ..
    "*.html"
    )
print ("lwarpmk: Done.")

-- lwarpmk with no argument :

elseif (arg[1] == nil) then
printhelp ()

-- lwarpmk -h or lwarpmk --help :

elseif (arg[1] == "-h" ) or (arg[1] == "--help") then
printusage ()

else
print ("lwarpmk: Unknown command \""..arg[1].."\".\n")
printhelp ()
end

end -- not --version
