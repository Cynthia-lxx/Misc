$procs = Get-CimInstance Win32_Process | Where-Object {
    $_.Name -match 'python|g\+\+|cc1plus|gdb|as\.exe|ld\.exe|collect2'
}
$procs | Select-Object ProcessId, Name, CommandLine | Format-List
