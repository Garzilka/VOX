#include <iostream>
#include "Data/Common.h"
#include "Data/AsioThreads.h"
#include "Core.h"


int main()
{
    //setlocale(LC_ALL, "Russian");
    SetConsoleOutputCP(CP_UTF8);          // вывод в консоль — UTF‑8
    SetConsoleCP(CP_UTF8);
    cout << "Сервер старт!\n";
    cout << "Запуск пулов потоков...\n";

    Core server;
    AsioThreadPool asioPool(server.getIoContext(), 2); // Пул Чат менеджера
    asioPool.start();
    if (!server.startServer(55)) 
    {
        return 1;
    }

    cout << "Сервер запущен\n";

    while (true) 
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    server.stopServer();
    return 0;
}