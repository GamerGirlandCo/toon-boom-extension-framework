param (
	[Parameter(Mandatory = $true)]
	[string]$DllPath,
	[Parameter(Mandatory = $true)]
	[string]$OutputPath
)

# Ensure DUMPBIN is available (must be run from VS Developer Command Prompt or similar environment)

$dllName = Split-Path -Path $DllPath -Leaf
$baseName = [System.IO.Path]::GetFileNameWithoutExtension($dllName)
$defFile = Join-Path -Path $OutputPath -ChildPath "$baseName.def"
$exportsFile = Join-Path -Path $OutputPath -ChildPath "$baseName-exports.txt"

Write-Host "Dumping exports to $exportsFile..."
# Execute dumpbin and redirect output to a temporary file
dumpbin /exports $DllPath > $exportsFile

Write-Host "Creating $defFile..."
# Start the .def file content
"LIBRARY $baseName" | Out-File -FilePath $defFile -Encoding ASCII
"EXPORTS" | Out-File -FilePath $defFile -Encoding ASCII -Append

# Process the dumpbin output to extract function names
# The output format can vary, so the token skip might need adjustment
# This example assumes the format where the name is the 4th token after skipping initial lines.
Get-Content $exportsFile | ForEach-Object {
	if ($_ -match '^\s+\d+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+([^\s=]+)') {
		$functionName = $matches[1]
		# Handle C++ name mangling if necessary (often, mangled names are put directly in the DEF)
		# For C-style linkage, names appear as expected.
		"$functionName" | Out-File -FilePath $defFile -Encoding ASCII -Append
	}
}

# Clean up the temporary exports file
Remove-Item $exportsFile

Write-Host "Successfully generated $defFile"
lib /def:$defFile /out:$OutputPath/$baseName.lib /machine:x64
# Example Usage (Run within a Developer PowerShell/Command Prompt):
# New-DefFileFromDll -DllPath "C:\Path\To\Your\library.dll"
