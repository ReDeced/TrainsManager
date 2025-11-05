#include <iostream>
#include "manager.h"

int main() {
    Manager manager("dataset.json");

    while (true) {
        std::cout << "\n===== СИСТЕМА ЖД БИЛЕТОВ =====\n";
        std::cout << "1. Регистрация пользователя\n";
        std::cout << "2. Вход по паспорту\n";
        std::cout << "3. Выход из программы\n";
        std::cout << "Выберите действие: ";

        int choice = 0;
        std::cin >> choice;

        // --- сброс ошибок и очистка потока ---
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ошибка ввода. Попробуйте снова.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 3) {
            std::cout << "Программа завершена.\n";
            break;
        }

        User* current_user = nullptr;

        if (choice == 1) {
            current_user = &manager.register_user();
        } else if (choice == 2) {
            u_int64_t passport_id;
            std::cout << "Введите серию и номер паспорта (10 цифр): ";
            std::cin >> passport_id;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Ошибка: некорректный ввод.\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            current_user = manager.get_user(passport_id);
            if (!current_user) {
                std::cout << "Ошибка: пользователь не найден.\n";
                continue;
            }
        } else {
            std::cout << "Неверный выбор.\n";
            continue;
        }

        // --- Меню пользователя ---
        while (true) {
            std::cout << "\n===== МЕНЮ ПОЛЬЗОВАТЕЛЯ =====\n";
            std::cout << "1. Купить билет\n";
            std::cout << "2. Показать билеты\n";
            std::cout << "3. Выйти из аккаунта\n";
            std::cout << "4. Закрыть программу\n";
            std::cout << "Выберите действие: ";

            int user_choice = 0;
            std::cin >> user_choice;

            // --- сброс ошибок и очистка потока ---
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Ошибка ввода. Попробуйте снова.\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (user_choice == 4) {
                std::cout << "Программа завершена.\n";
                return 0;
            }

            if (user_choice == 3) {
                std::cout << "Вы вышли из аккаунта.\n";
                break; // выход в главное меню
            }

            if (user_choice == 1) {
                std::cout << "\n===== ДОСТУПНЫЕ МАРШРУТЫ =====\n";
                manager.show_routes();

                int train_id = 0, car_id = 0;
                std::string from_station, to_station;

                std::cout << "Введите ID поезда: ";
                std::cin >> train_id;
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Ошибка: некорректный ID.\n";
                    continue;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout << "Введите номер вагона: ";
                std::cin >> car_id;
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Ошибка: некорректный номер.\n";
                    continue;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                std::cout << "Введите станцию отправления: ";
                std::getline(std::cin, from_station);

                std::cout << "Введите станцию назначения: ";
                std::getline(std::cin, to_station);

                try {
                    Ticket& ticket = manager.buy_ticket(*current_user, train_id, car_id, from_station, to_station);
                } catch (const std::runtime_error& e) {
                    std::cout << "Ошибка: " << e.what() << "\n";
                }

            } else if (user_choice == 2) {
                current_user->show_tickets();

                if (!current_user->get_tickets().empty()) {
                    std::cout << "\nСохранить билеты в файл? (д/н): ";
                    std::string save_choice;
                    std::cin >> save_choice;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    if (save_choice == "д" || save_choice == "Д") {
                        for (auto& ticket : current_user->get_tickets()) {
                            ticket.save_to_file();
                        }
                        std::cout << "Билеты сохранены.\n";
                    } else {
                        std::cout << "Билеты не сохранены.\n";
                    }
                } else {
                    std::cout << "У вас нет билетов для сохранения.\n";
                }

            } else {
                std::cout << "Неверный выбор.\n";
            }
        }
    }

    return 0;
}
