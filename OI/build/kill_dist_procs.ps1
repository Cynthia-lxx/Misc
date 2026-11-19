Get-CimInstance Win32_Process | Where-Object {
    ($_.ExecutablePath -like 'P:\Dev\OI\dist*') -or ($_.CommandLine -like '*OI\dist*')
} | ForEach-Object {
    "KILL pid=$($_.ProcessId) $($_.Name) :: $($_.CommandLine)"
    Stop-Process -Id $_.ProcessId -Force -ErrorAction SilentlyContinue
}
Start-Sleep -Seconds 2
"--- 剩余 dist 相关进程 ---"
Get-CimInstance Win32_Process | Where-Object {
    ($_.ExecutablePath -like 'P:\Dev\OI\dist*') -or ($_.CommandLine -like '*OI\dist*')
} | Select-Object ProcessId, Name | Format-Table -AutoSize
