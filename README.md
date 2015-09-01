# NVIDIA Hair Works Integration

NVIDIA Hair Wors の Unity インテグレーションです。  
使用にはやや複雑な手順が必要です。
Hair Works は基本的に再配布が認められていないため、各自個別に NVIDIA の開発者サイトから入手してプロジェクトへインポートする必要があります。
以下に手順を記します。  

1.  このパッケージをプロジェクトへインポート
2.  Hair Works SDK を入手
  * https://developer.nvidia.com/hairworks ここの Download -> HairWorks 1.1
  * ダウンロードには NVIDIA の開発者アカウントが必要です。アカウント作成は無料ですが承認が必要で、申請から 1 日前後時間がかかります。
3.  Hair Works SDK に含まれる必要なデータをプロジェクトへインポート
  * 

また、プラグインをビルドしたい場合は Hair Works SDK を Plugin\Externals に置きます。  
(Plugin\Externals\HairWorks-r1-1-212-distro\include に必要な .h ファイル群があれば大丈夫です)
