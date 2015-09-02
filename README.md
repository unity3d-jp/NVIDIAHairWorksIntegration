# NVIDIA Hair Works Integration

NVIDIA Hair Wors の Unity インテグレーションです。
MAYA や 3ds Max でオーサリングした Hair データを Unity で表示させることができます。

使用には少々面倒な手順が必要です。
Hair Works は基本的に再配布が認められていないため、各自個別に NVIDIA の開発者サイトから入手してプロジェクトへインポートする必要があります。
以下に手順を記します。  
(現状 SDK はバージョン 1.1 を前提としています)

1.  このパッケージをプロジェクトへインポート
2.  Hair Works SDK を入手
  * https://developer.nvidia.com/hairworks ここの Download -> HairWorks 1.1
  * ダウンロードには NVIDIA の開発者アカウントが必要です。アカウント作成は無料ですが、申請から作成されるまでに 1 日前後時間がかかります。
3.  Hair Works SDK に含まれる必要なデータをプロジェクトへインポート
  * HairWorks-r1-1-212-distro/bin/win64/GFSDK_HairWorks.win64.dll を Assets/HairWorksIntegration/Plugins の中にコピー

また、プラグインをビルドしたい場合は HairWorks-r1-1-212-distro ディレクトリをまるごと Plugin/Externals の中にコピーしてから Plugin/HairWorksIntegration.sln を開いてビルドします。
