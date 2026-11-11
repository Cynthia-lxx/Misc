# ============================================
# 批量转换 .ipynb 为 HTML 并合并为一个文件
# 适用于 Windows PowerShell 5.5
# ============================================

# 设置路径
$RootDir = Get-Location                          # 当前目录为根目录
$OutputDir = Join-Path $RootDir "html_output"    # 临时存放单个HTML
$FinalFile = Join-Path $RootDir "merged_all.html" # 最终合并文件

# 创建输出目录
if (-not (Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir | Out-Null
}

# 查找所有 .ipynb 文件（递归子目录）
$Notebooks = Get-ChildItem -Path $RootDir -Recurse -Filter "*.ipynb" | Sort-Object FullName

if ($Notebooks.Count -eq 0) {
    Write-Host "? 没有找到任何 .ipynb 文件！" -ForegroundColor Red
    exit 1
}

Write-Host "? 找到 $($Notebooks.Count) 个 notebook 文件" -ForegroundColor Cyan

# ============================================
# 第一步：逐个转换为 HTML
# ============================================
$HtmlFiles = @()

foreach ($nb in $Notebooks) {
    $RelativePath = $nb.FullName.Substring($RootDir.Length + 1)
    Write-Host "?? 正在转换: $RelativePath" -ForegroundColor Yellow

    # 生成输出文件名：用相对路径替换路径分隔符为 "__"，避免重名
    $SafeName = $RelativePath -replace '\\', '__' -replace '/', '__' -replace '\.ipynb$', ''
    $OutHtml = Join-Path $OutputDir "$SafeName.html"

    # 调用 nbconvert，使用 lab 模板（好看）
    & jupyter nbconvert --to html --template lab --output "$SafeName.html" --output-dir $OutputDir $nb.FullName 2>&1 | Out-Null

    if (Test-Path $OutHtml) {
        $HtmlFiles += $OutHtml
        Write-Host "   ? 生成成功: $SafeName.html" -ForegroundColor Green
    } else {
        Write-Host "   ? 转换失败: $RelativePath" -ForegroundColor Red
    }
}

if ($HtmlFiles.Count -eq 0) {
    Write-Host "? 没有成功生成任何 HTML 文件！" -ForegroundColor Red
    exit 1
}

Write-Host "`n? 共生成 $($HtmlFiles.Count) 个 HTML 文件" -ForegroundColor Cyan

# ============================================
# 第二步：合并所有 HTML（提取 body 内容）
# ============================================
Write-Host "`n?? 正在合并 HTML..." -ForegroundColor Cyan

# 构建合并后的 HTML 头部
$HtmlHeader = @'
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>合并的所有 Notebook</title>
    <style>
        /* 打印分页 */
        .notebook-page {
            page-break-after: always;
        }
        .notebook-page:last-child {
            page-break-after: avoid;
        }
        .notebook-title {
            font-size: 24px;
            font-weight: bold;
            padding: 20px 0 10px 0;
            border-bottom: 3px solid #333;
            margin-bottom: 20px;
            color: #1a1a1a;
        }
        /* 代码字体好看一点 */
        pre, code, .output pre, .input_area pre {
            font-family: "Fira Code", "Consolas", "Lucida Console", monospace !important;
            font-size: 13px;
        }
        /* 打印时去掉多余背景 */
        @media print {
            body { margin: 0; padding: 15px; }
            .notebook-page { page-break-after: always; }
        }
    </style>
</head>
<body>
'@

# 构建合并后的 HTML 尾部
$HtmlFooter = @'
</body>
</html>
'@

# 开始构建内容
$BodyContent = New-Object System.Text.StringBuilder
$BodyContent.AppendLine($HtmlHeader) | Out-Null

$count = 0
foreach ($htmlFile in $HtmlFiles) {
    $count++
    $fileName = [System.IO.Path]::GetFileNameWithoutExtension($htmlFile)
    # 把文件名中的 "__" 还原成路径分隔符，便于阅读
    $DisplayName = $fileName -replace '__', ' / '
    Write-Host "   ?? 合并: $DisplayName" -ForegroundColor Gray

    # 读取 HTML 内容
    $htmlContent = Get-Content -Path $htmlFile -Raw -Encoding UTF8

    # 提取 <body> 标签内的内容（去掉 <body> 和 </body> 本身）
    if ($htmlContent -match '(?s)<body[^>]*>(.*?)</body>') {
        $bodyInner = $Matches[1]
    } else {
        # 如果没有 body，就用整个内容（兜底）
        $bodyInner = $htmlContent
    }

    # 添加一个分页容器
    [void]$BodyContent.AppendLine('<div class="notebook-page">')
    [void]$BodyContent.AppendLine("<div class='notebook-title'>?? $DisplayName</div>")
    [void]$BodyContent.AppendLine($bodyInner)
    [void]$BodyContent.AppendLine('</div>')
}

$BodyContent.AppendLine($HtmlFooter) | Out-Null

# 写入最终文件
Set-Content -Path $FinalFile -Value $BodyContent.ToString() -Encoding UTF8

Write-Host "`n?? 合并完成！最终文件: $FinalFile" -ForegroundColor Green
Write-Host "?? 文件大小: $([math]::Round((Get-Item $FinalFile).Length / 1KB, 2)) KB" -ForegroundColor Cyan
Write-Host "?? 打开 $FinalFile 后按 Ctrl+P 即可批量打印（每份 notebook 自动分页）" -ForegroundColor Yellow