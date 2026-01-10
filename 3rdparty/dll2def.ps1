param (
	[parameter(mandatory = $true)]
	[string]$dllpath,
	[parameter(mandatory = $true)]
	[string]$outputpath
)

# ensure dumpbin is available (must be run from vs developer command prompt or similar environment)

$dllname = split-path -path $dllpath -leaf
$basename = [system.io.path]::getfilenamewithoutextension($dllname)
$deffile = join-path -path $outputpath -childpath "$basename.def"
$exportsfile = join-path -path $outputpath -childpath "$basename-exports.txt"

write-host "dumping exports to $exportsfile..."
# execute dumpbin and redirect output to a temporary file
dumpbin /exports $dllpath > $exportsfile

write-host "creating $deffile..."
# start the .def file content
"library $basename" | out-file -filepath $deffile -encoding ascii
"exports" | out-file -filepath $deffile -encoding ascii -append

# process the dumpbin output to extract function names
# the output format can vary, so the token skip might need adjustment
# this example assumes the format where the name is the 4th token after skipping initial lines.
get-content $exportsfile | foreach-object {
	if ($_ -match '^\s+\d+\s+[0-9a-fa-f]+\s+[0-9a-fa-f]+\s+([^\s=]+)') {
		$functionname = $matches[1]
		# handle c++ name mangling if necessary (often, mangled names are put directly in the def)
		# for c-style linkage, names appear as expected.
		"$functionname" | out-file -filepath $deffile -encoding ascii -append
	}
}

# clean up the temporary exports file
remove-item $exportsfile

write-host "successfully generated $deffile"
lib /def:$deffile /out:$outputpath/$basename.lib /machine:x64
# example usage (run within a developer powershell/command prompt):
# new-deffilefromdll -dllpath "c:\path\to\your\library.dll"
