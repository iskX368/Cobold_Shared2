// TabSepText2Array.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <conio.h>

//Igorで出力した二次元タブ区切り数表をC++の配列としてそのままコピペできるようにする


constexpr char output[] = "TabSepText2Array_Output.txt";
constexpr char sep = ',';

int main()
{
    std::ifstream ifs;
    std::ofstream ofs;

    try {
        std::string filename;


        std::cout << "File name : ";
        std::cin >> filename;
        std::cout << std::endl;

        
        ifs.open(filename);
        if (!ifs) {
            throw std::invalid_argument("InputFile open failure");
        }

        
        ofs.open(output);
        if (!ofs) {
            throw std::exception("OutputFile open failure");
        }


        std::stringstream ss;
        std::string line, elm;

        ofs << "{";
        while (std::getline(ifs, line)) {
            ss.str("");
            ss.clear(std::stringstream::goodbit);

            ss << line;

            ofs << "{";

            while (!ss.eof()) {
                ss >> elm;
                ofs << elm << sep;
            }

            ofs << "}" << sep << std::endl;

        }
        ofs << "}" << std::endl;

        ofs.close();
        ifs.close();

        std::cout << "Finished" << std::endl;

        std::cout << "Press any key to exit...";
        _getch();
        std::cout << std::endl;

        return 0;
    }
    catch (std::exception& ex) {
        if (ofs) {
            ofs.close();
        }
        if (ifs) {
            ifs.close();
        }
        
        std::cerr << ex.what() << std::endl;

        std::cout << "Press any key to exit...";
        _getch();
        std::cout << std::endl;

        return 1;
    }
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
