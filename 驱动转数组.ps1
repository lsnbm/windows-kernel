param(
  [string]$InputFile  = "ataport.sys",
  [string]$OutputFile = "driver_data.h",
  [string]$ArrayName  = "Driver_sys"
)

# 读取所有字节
[Byte[]]$bytes = Get-Content -Path $InputFile -Encoding Byte

# 打开输出文件
$out = New-Object System.IO.StreamWriter($OutputFile, $false, [System.Text.Encoding]::ASCII)

# 写 C++ 数组头
$out.WriteLine("// 自动生成：来自 $InputFile")
$out.WriteLine("unsigned char ${ArrayName}[] = {")
 
# 每行写 16 个字节
for ($i = 0; $i -lt $bytes.Length; $i++) {
    $hex = "{0:X2}" -f $bytes[$i]
    $comma = if ( $i -lt $bytes.Length - 1 ) { "," } else { "" }
    $out.Write("0x$hex$comma")
    if ((($i + 1) % 16) -eq 0) { $out.WriteLine() }
    else { $out.Write(" ") }
}

# 结束大括号和长度变量
$out.WriteLine()
$out.WriteLine("};")
$out.WriteLine("unsigned int ${ArrayName}_len = sizeof(${ArrayName});")

$out.Close()

Write-Host "已生成 $OutputFile (长度 $($bytes.Length) 字节)"
