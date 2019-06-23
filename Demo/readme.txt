使用说明：
1.安装支持库文件，双击 install 文件夹中的 vc_redist.x64
2.将输入文件放置data文件夹内，命名为input.png
3.运行相关程序

程序说明：
/-------处理标准QR code --------/
标准的QRcode：position aligmnet 等用于定位的pattern正确

1.CarvingQR_Pretreat.exe
//QR code的预处理
//输入：QRcode图像
//输出：position pattern以对角线联通的QRcode

2.CarvingQR_ConnectOnly.exe
//QR code的连通处理（需要计算loss，较慢）
//输入：QRcode图像
//输出：全联通的QRcode

3.CarvingQR_L.exe
//QR code 的预处理与连通处理（计算loss）
//输入：QRcode图像
//输出：全联通的QRcode
/-------处理标准QR code --------/

/-------处理不标准的QR code--------/
1.CarvingQR_SP
//QR code的连通处理（最短路径）
//输入：QRcode图像
//输出：全联通的QRcode图像
/-------处理不标准的QR code--------/
