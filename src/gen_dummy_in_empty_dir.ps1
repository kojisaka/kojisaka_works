cmd /c dir /s /b /ad | ForEach-Object {
	$d = cmd /c dir /b $_
	if ($d -eq $null) { New-Item -Path "$_\.dummy" >$null }
}
Get-ChildItem -Path . -Filter .gitattributes -Recurse -File | Remove-Item -Force
Get-ChildItem -Path . -Filter .gitignore     -Recurse -File | Remove-Item -Force
