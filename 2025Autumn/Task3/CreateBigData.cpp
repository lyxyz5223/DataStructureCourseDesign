// #include <iostream>
// #include <fstream>
// #include <random>
// #include <string>

// int main()
// {
//   std::string filename;
//   double sizeGB;
//   std::cout << "请输入要生成的文件名: ";
//   std::cin >> filename;
//   std::cout << "请输入文件大小(GB): ";
//   std::cin >> sizeGB;

//   const size_t GB = 1024ull * 1024 * 1024;
//   size_t totalSize = static_cast<size_t>(sizeGB * GB);
//   const size_t bufferSize = 1024 * 1024; // 1MB缓冲区
//   char *buffer = new char[bufferSize];

//   std::random_device rd;
//   std::mt19937 gen(rd());
//   std::uniform_int_distribution<> dis(0, 255);

//   std::ofstream out(filename, std::ios::binary);
//   if (!out)
//   {
//     std::cerr << "无法打开文件: " << filename << std::endl;
//     delete[] buffer;
//     return 1;
//   }

//   size_t written = 0;
//   while (written < totalSize)
//   {
//     size_t toWrite = std::min(bufferSize, totalSize - written);
//     for (size_t i = 0; i < toWrite; ++i)
//     {
//       buffer[i] = static_cast<char>(dis(gen));
//     }
//     out.write(buffer, toWrite);
//     written += toWrite;
//     // 可选：显示进度
//     if (written % (100 * bufferSize) == 0)
//     {
//       std::cout << "\r已写入: " << written / GB << " GB" << std::flush;
//     }
//   }
//   std::cout << "\n文件生成完毕: " << filename << std::endl;

//   out.close();
//   delete[] buffer;
//   return 0;
// }

#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <limits>

int main()
{
  std::string filename;
  double sizeGB;
  std::cout << "6请输入要生成的文件名: ";
  std::cin >> filename;
  std::cout << "请输入文件大小(GB): ";
  std::cin >> sizeGB;

  const size_t GB = 1024ull * 1024 * 1024;
  size_t totalSize = static_cast<size_t>(sizeGB * GB);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, std::numeric_limits<int>::max());

  std::ofstream out(filename, std::ios::out);
  if (!out)
  {
    std::cerr << "无法打开文件: " << filename << std::endl;
    return 1;
  }

  size_t written = 0;
  while (written < totalSize)
  {
    int num = dis(gen);
    std::string line = std::to_string(num) + "\n";
    out << line;
    written += line.size();

    // 可选：显示进度
    if (written % (100 * 1024 * 1024) < line.size()) // 每写入约100MB显示一次
    {
      std::cout << "\r已写入: " << written / GB << " GB" << std::flush;
    }
  }
  std::cout << "\n文件生成完毕: " << filename << std::endl;

  out.close();
  return 0;
}
