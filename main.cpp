#include <iostream>
#include "manager.h"

int main() {
    Manager manager("dataset.json");

    std::cout << "===== СИСТЕМА ЖД БИЛЕТОВ =====\n";
    std::cout << "1. Регистрация пользователя\n";
    std::cout << "2. Вход по паспорту\n";
    std::cout << "Выберите действие: ";

    int choice;
    std::cin >> choice;
    std::cin.ignore();

    User* current_user = nullptr;

    if (choice == 1) {
        current_user = &manager.register_user();
    } else if (choice == 2) {
        u_int64_t passport_id;
        std::cout << "Введите номер паспорта (10 цифр): ";
        std::cin >> passport_id;
        std::cin.ignore();

        current_user = manager.get_user(passport_id);
        if (!current_user) {
            std::cout << "Ошибка: пользователь не найден.\n";
            return 0;
        }
    } else {
        std::cout << "Неверный выбор.\n";
        return 0;
    }

    std::cout << "\n===== МЕНЮ ПОЛЬЗОВАТЕЛЯ =====\n";
    std::cout << "1. Показать билеты\n";
    std::cout << "2. Выход\n";
    std::cout << "Выберите действие: ";

    int user_choice;
    std::cin >> user_choice;
    std::cin.ignore();

    if (user_choice == 1) {
        current_user->show_tickets();
    } else {
        std::cout << "Выход.\n";
    }

    return 0;
}
