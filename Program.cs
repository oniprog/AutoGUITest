using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.IO;
using System.Drawing;
using System.Drawing.Imaging;

/*
 * 自動GUIテスト用プログラム
 * 
 * このプログラムを直接書き換えて，ご使用ください．．．
 *
 */

namespace AutoGUITest
{
    class Program
    {
        // 生成するHTML内のURLにつけるパス
        private static String URL_ROOT = "file:///E:/myhome/OpenSource/AutoGUITest/TestScript/";

        // テストスクリプトのあるフォルダの一つ上
        // このフォルダに，テストスクリプトのフォルダ一覧があります．
        private static String strScriptFolder = @"..\..\TestScript";

        // 対象プログラムのパス
        private static String strTargetExePath = "..\\..\\Debug\\SampleGUITest.exe";

        // テスト用スクリプト名
        private static String strTestScriptName = @"test.lua";

        // 対象プログラムをKillするまでの時間．この時間内に処理が完了する必要がある．
        private static int nWaitForKill = 30 * 60 * 1000; // (ms)

        // 生成するHTMLのファイル名
        private static String strHTMLName = "test_result.html";

        // 正解データがあるフォルダ名
        private static String strCorrectDataFolderName = "correct";

        // 比較した結果生成された画像を入れるフォルダ名
        private static String strCmpImageFolderName = "tmp";

        // 対象プログラムが結果を保存用フォルダ
        private static String strResultFolderName = "result";

        // 対象プログラムの自動GUIテスト用起動オプション
        private static String strAutoTestCommand = "/autotest";

        // 
        static void Main(string[] args)
        {
            // 実行する自動GUIテストスクリプトのフィルタ
            String strTargetDirFilter = "*";    // 引数に何もなければ全部のテストを実行
            if (args.Length > 0)
                strTargetDirFilter = args[0];   // 引数を受け取ってフィルタとする

            // 対象プログラムが存在するかをチェックする
            if (!File.Exists(strTargetExePath))
            {
                System.Console.WriteLine("対象EXEが見つかりません");
                return;
            }

            // 対象プログラムのパスをフルパスに変換する
            strTargetExePath = new FileInfo(strTargetExePath).FullName;

            System.Environment.CurrentDirectory = strScriptFolder;
            String strPushDir = System.Environment.CurrentDirectory;

            // テスト対象のホームページ作成
            using (HTMLWriter hw = new HTMLWriter(strHTMLName) )
            {
                // テストを実行していく
                var dirlist = Directory.GetDirectories(".", strTargetDirFilter);
                foreach (var dirnameM in dirlist)
                {
                    System.Environment.CurrentDirectory = strPushDir;

                    // 余分な文字を外す (.\)の部分
                    String strDirname = new FileInfo(dirnameM).Name;

                    // テストスクリプトがあるかをチェックする
                    if (!File.Exists(strDirname + "\\" + strTestScriptName ))
                        continue;

                    // ディレクトリを保存する
                    strPushDir = System.Environment.CurrentDirectory;

                    // 一時ファイル保存用フォルダ
                    Directory.CreateDirectory(strDirname + "\\" + strCmpImageFolderName);

                    // 結果保存フォルダに移動する
                    Directory.CreateDirectory(strDirname + "\\" + strResultFolderName );

                    System.Environment.CurrentDirectory += "\\" + strDirname + "\\" + strResultFolderName ;

                    // プログラム実行
                    System.Console.WriteLine("Run " + strDirname);
                    hw.WriteDirName(strDirname);

                    // 説明付加
                    hw.AddReadMe();

                    // 対象プログラムを実行する
                    var process = System.Diagnostics.Process.Start(strTargetExePath, strAutoTestCommand + " \"..\\"+strTestScriptName+"\"" );
                    process.WaitForExit(nWaitForKill);

                    // 一定時間経っても終了していなかったら消す
                    if (!process.HasExited)
                    {
                        process.Kill();
                        hw.KillProcess();
                    }

                    // 比較対象のフォルダと画像を比較する
                    String strCorrectDataPath = "..\\" + strCorrectDataFolderName;
                    CompareDirectories(hw, strCorrectDataPath, strDirname);

                    hw.Flush();
                }

                hw.CloseHTML();
            }
        }


        // 正解データと比較して結果を生成する
        private static void CompareDirectories(HTMLWriter hw, String strCorrectDataPath, String strDirName )
        {

            if (Directory.Exists(strCorrectDataPath))
            {
                foreach (var cmpfile in Directory.GetFiles(strCorrectDataPath))
                {
                    // 比較正解データが.pngかをチェックする
                    if (!cmpfile.EndsWith(".png"))
                        continue;

                    // 比較先が存在するかをチェックする
                    String strCmpFileName = new FileInfo(cmpfile).Name; // 名前だけを取り出す
                    if (File.Exists(strCmpFileName))
                    {
                        CompareImage( hw, strDirName, strCmpFileName, strCorrectDataPath + "\\" + strCmpFileName );
                    }
                    else
                    {
                        hw.MissingCompareImage( strDirName, strCmpFileName );
                    }

                    hw.WriteLine();
                }
            }
        }

        ///  画像を比較する
        private static void CompareImage( HTMLWriter hw, String strDirName, String strCmpFileName, String strCorrectImagePath ) {

            // 結果ファイルとの比較の作成
            Bitmap imageResult = new Bitmap(Image.FromFile(strCmpFileName));
            Bitmap imageCorrect = new Bitmap(Image.FromFile(strCorrectImagePath));

            if (imageResult.Size != imageCorrect.Size)
            {
                hw.ImageSizeDifferent(strDirName, strCmpFileName);
            }
            else
            {
                // サイズが同じ．比較画像の作成
                double dErrorSum = 0;
                int nErrorCnt = 0;
                var sizeImage = imageResult.Size;
                Bitmap imageCompare = new Bitmap(sizeImage.Width, sizeImage.Height);
                for (int iy = 0; iy < sizeImage.Height; ++iy)
                {
                    for (int ix = 0; ix < sizeImage.Width; ++ix)
                    {
                        var pixel1 = imageResult.GetPixel(ix, iy);
                        var pixel2 = imageCorrect.GetPixel(ix, iy);
                        var pixel3 = Color.FromArgb(pixel1.R ^ pixel2.R, pixel1.G ^ pixel2.G, pixel1.B ^ pixel2.B);

                        imageCompare.SetPixel(ix, iy, pixel3);
                        dErrorSum += pixel3.R + pixel3.G + pixel3.B;
                        ++nErrorCnt;
                    }
                }

                // 比較した結果の画像を保存
                String strTmpPath = "..\\" + strCmpImageFolderName + "\\" + strCmpFileName;
                imageCompare.Save(strTmpPath, ImageFormat.Png);

                // エラーとみなす閾値を超えているかのチェック
                bool bError = dErrorSum >= 10000;

                if (!bError)
                {
                    // エラーが少ないときの対処
                    // 何も表示しない
    #if false
    //                hw.WriteNoError(strDirName, strCmpFileName);
    #else
                    return;
    #endif
                }
                else
                {
                    hw.WriteErrorValue(dErrorSum);  // エラー値を表示する
                    hw.WriteTestResult(strDirName, strCmpFileName);
                }
            }
        }

        /** HTML出力関係をまとめた */
        public class HTMLWriter : IDisposable
        {
            private StreamWriter mSW;

            public HTMLWriter(String strPath)
            {
                this.mSW = new StreamWriter(strPath, false/*新規作成*/, Encoding.UTF8);

                WriteHead();
            }

            public void Dispose()
            {
                if (this.mSW != null) { 
                    this.mSW.Dispose();
                    this.mSW = null;
                }
            }

            // ヘッダー部分
            private void WriteHead()
            {
                mSW.WriteLine("<!DOCTYPE html><HTML><HEAD><meta charset=\"UTF-8\"></HEAD><BODY>");
                mSW.WriteLine("<HR><BR><H1>自動GUIテストです  " + DateTime.Now.ToLocalTime().ToShortDateString() + " "
                    + DateTime.Now.ToLocalTime().ToShortTimeString() + " </H1><HR><BR>");
            }

            // ディレクトリ名を記述する
            public void WriteDirName(String strDirname)
            {
                mSW.WriteLine("<H1>" + strDirname + "</H1>");
            }

            // 説明を付加する
            public void AddReadMe()
            {
                if (File.Exists("..\\readme.txt"))
                {
                    using (StreamReader sr = new StreamReader("..\\readme.txt", Encoding.GetEncoding("sjis")))
                    {
                        mSW.WriteLine("<H2>" + sr.ReadToEnd() + "</H2>");
                    }
                }
            }

            // プロセスを殺したことを記録する
            public void KillProcess()
            {
                mSW.WriteLine("<H2>Process is Killed</H2>");
            }

            // 比較対象のファイルが生成されていないとき
            public void MissingCompareImage( String strDirName, String strCmpFileName ) {

                //
                mSW.WriteLine("<H2>Missing " + strCmpFileName + "</H2>");

                // 正解ファイルへのリンク
                mSW.WriteLine("<IMG SRC=\"" + URL_ROOT + strDirName + "/correct/" + strCmpFileName + "\">");
            }

            ///  ラインを引く
            public void WriteLine()
            {
                mSW.WriteLine("<BR><HR><BR>");
            }


            // 画像サイズが異なるとき
            public void ImageSizeDifferent(String strDirname, String strCmpFileName )
            {
                // サイズが異なる
                mSW.WriteLine("<H3>Size is diffrent. Images cannot be compared.</H3>");

                // テスト結果ファイルへのリンク
                mSW.WriteLine("<IMG SRC=\"" + URL_ROOT + strDirname + "/" + strResultFolderName +"/" + strCmpFileName + "\">");

                // 正解ファイルへのリンク
                mSW.WriteLine("<IMG SRC=\"" + URL_ROOT + strDirname + "/" + strCorrectDataFolderName + "/" + strCmpFileName + "\">");
            }

            // 画像比較したときのエラー値の出力
            public void WriteErrorValue(double dErrorValue)
            {
                mSW.WriteLine("<H3> Error Value " + ("" + (int)dErrorValue) + "<H3><BR>");
            }

            // エラーがないときの結果の表示
            public void WriteNoError(String strDirName, String strCmpFileName)
            {
                // テスト結果ファイルへのリンク
                mSW.WriteLine("<IMG WIDTH=\"200\" HEIGHT=\"200\" SRC=\"" + URL_ROOT + strDirName + "/tmp/" + strCmpFileName + "\">");

                // テスト結果ファイルへのリンク
                mSW.WriteLine("<IMG WIDTH=\"200\" HEIGHT=\"200\" SRC=\"" + URL_ROOT + strDirName + "/result/" + strCmpFileName + "\">");

                // 正解ファイルへのリンク
                mSW.WriteLine("<IMG WIDTH=\"200\" HEIGHT=\"200\" SRC=\"" + URL_ROOT + strDirName + "/correct/" + strCmpFileName + "\">");

            }

            // テスト結果の表示
            public void WriteTestResult(String strDirName, String strCmpFileName)
            {

                // テスト結果ファイルへのリンク
                mSW.WriteLine("<IMG SRC=\"" + URL_ROOT + strDirName + "/"+strCmpImageFolderName+"/" + strCmpFileName + "\">");

                mSW.WriteLine("<BR>");

                // テスト結果ファイルへのリンク
                mSW.WriteLine("<IMG SRC=\"" + URL_ROOT + strDirName + "/" + strResultFolderName + "/" + strCmpFileName + "\">");

                // 正解ファイルへのリンク
                mSW.WriteLine("<IMG SRC=\"" + URL_ROOT + strDirName + "/" + strCorrectDataFolderName +"/" + strCmpFileName + "\">");
            }

            // すぐ出力する
            public void Flush()
            {
                mSW.Flush();
            }

            // HTMLを閉じる
            public void CloseHTML()
            {
                mSW.WriteLine("</BODY></HTML>");
            }
        }
    }
}
