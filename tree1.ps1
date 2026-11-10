# TreeLimited.ps1 - 限制深度和每层显示数量的树形目录查看器

param(
    [Parameter(Position=0)]
    [string]$Path = ".",
    
    [switch]$f,  # 显示文件
    [int]$MaxDepth = 4,
    [int]$MaxItems = 20
)

# 初始化计数器
$script:totalItems = 0
$script:totalFiles = 0
$script:totalDirs = 0

# 判断是否显示文件
$showFiles = $f.IsPresent

# 获取绝对路径
$rootPath = Resolve-Path $Path -ErrorAction Stop

function Write-TreeLimited {
    param(
        [string]$CurrentPath,
        [int]$Depth = 0,
        [string]$Prefix = "",
        [bool]$IsLast = $true
    )
    
    # 超出最大深度则停止
    if ($Depth -ge $MaxDepth) {
        return
    }
    
    # 获取当前目录下的所有项
    $items = Get-ChildItem -Path $CurrentPath -ErrorAction SilentlyContinue
    
    # 分离目录和文件
    $dirs = $items | Where-Object { $_.PSIsContainer } | Sort-Object Name
    $files = if ($showFiles) {
        $items | Where-Object { -not $_.PSIsContainer } | Sort-Object Name
    } else {
        @()
    }
    
    # 合并列表（目录在前），使用 @() 强制转为数组后拼接
    $allItems = @($dirs) + @($files)
    
    # 限制数量
    $totalCount = $allItems.Count
    $displayItems = $allItems | Select-Object -First $MaxItems
    $hasMore = $totalCount -gt $MaxItems
    
    # 构建显示前缀
    $indent = if ($IsLast) { "    " } else { "│   " }
    
    # 遍历要显示的项目
    $itemCount = $displayItems.Count
    for ($i = 0; $i -lt $itemCount; $i++) {
        $item = $displayItems[$i]
        $isLastItem = ($i -eq $itemCount - 1) -and (-not $hasMore)
        
        # 构建连接符
        $connector = if ($isLastItem) { "└── " } else { "├── " }
        
        # 输出当前项
        $displayName = $item.Name
        if ($item.PSIsContainer) {
            Write-Host "$Prefix$connector$displayName/" -ForegroundColor Cyan
            $script:totalDirs++
        } else {
            Write-Host "$Prefix$connector$displayName" -ForegroundColor Gray
            $script:totalFiles++
        }
        $script:totalItems++
        
        # 如果是目录且未达到最大深度，递归进入
        if ($item.PSIsContainer) {
            $newPrefix = if ($isLastItem) { "$Prefix    " } else { "$Prefix│   " }
            Write-TreeLimited -CurrentPath $item.FullName -Depth ($Depth + 1) -Prefix $newPrefix -IsLast $isLastItem
        }
    }
    
    # 如果有省略的项目，显示省略提示
    if ($hasMore) {
        $skipped = $totalCount - $MaxItems
        $skipText = if ($skipped -gt 0) { " ... 还有 $skipped 项未显示" } else { " ..." }
        $connector = if ($displayItems.Count -gt 0) { "└── " } else { "─── " }
        Write-Host "$Prefix$connector[省略$skipText]" -ForegroundColor Yellow
    }
}

# 主程序开始
Write-Host "`n=== TreeLimited 开始 ===" -ForegroundColor Green
Write-Host "路径: $rootPath" -ForegroundColor Green
Write-Host "最大深度: $MaxDepth 层" -ForegroundColor Green
Write-Host "每层最大显示: $MaxItems 项" -ForegroundColor Green
if ($showFiles) { Write-Host "模式: 显示文件和目录" -ForegroundColor Green }
else { Write-Host "模式: 仅显示目录" -ForegroundColor Green }
Write-Host ""

# 显示根目录
$rootName = Split-Path $rootPath -Leaf
if (-not $rootName) { $rootName = $rootPath.Path }
Write-Host $rootName -ForegroundColor Magenta

# 开始遍历
$firstLevelItems = Get-ChildItem -Path $rootPath -ErrorAction SilentlyContinue
$dirs = $firstLevelItems | Where-Object { $_.PSIsContainer } | Sort-Object Name
$files = if ($showFiles) {
    $firstLevelItems | Where-Object { -not $_.PSIsContainer } | Sort-Object Name
} else { @() }
$allItems = @($dirs) + @($files)

$totalCount = $allItems.Count
$displayItems = $allItems | Select-Object -First $MaxItems
$hasMore = $totalCount -gt $MaxItems

# 显示第一层
$itemCount = $displayItems.Count
for ($i = 0; $i -lt $itemCount; $i++) {
    $item = $displayItems[$i]
    $isLastItem = ($i -eq $itemCount - 1) -and (-not $hasMore)
    
    $connector = if ($isLastItem) { "└── " } else { "├── " }
    
    if ($item.PSIsContainer) {
        Write-Host "$connector$($item.Name)/" -ForegroundColor Cyan
        $script:totalDirs++
        $newPrefix = if ($isLastItem) { "    " } else { "│   " }
        Write-TreeLimited -CurrentPath $item.FullName -Depth 1 -Prefix $newPrefix -IsLast $isLastItem
    } else {
        Write-Host "$connector$($item.Name)" -ForegroundColor Gray
        $script:totalFiles++
    }
    $script:totalItems++
}

if ($hasMore) {
    $skipped = $totalCount - $MaxItems
    Write-Host "└── [省略 $skipped 项]" -ForegroundColor Yellow
}

# 统计信息
Write-Host ""
Write-Host "=== 统计 ===" -ForegroundColor Green
Write-Host "总计: $script:totalItems 项 (目录: $script:totalDirs, 文件: $script:totalFiles)" -ForegroundColor Green