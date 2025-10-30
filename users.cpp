#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include "trains.h"
#include "users.h"

void Ticket::print_ticket() const {
    std::cout << "Билет на поезд: " << train_id << std::endl;
    std::cout << "Маршрут: " << from_station << " -> " << to_station << std::endl;
    std::cout << "Класс вагона: " << car_id << std::endl;
    std::cout << "Цена: " << price << " руб." << std::endl;
}

void Ticket::save_to_file() const {
    std::string filename = "ticket_" + std::to_string(train_id) + "_" + owner->get_passport().name + ".txt";

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для записи\n";
        return;
    }

    // Записываем информацию о билете
    file << "====== Билет на поезд ======\n";
    file << "Владелец: " << owner->get_passport().name << "\n";
    file << "ID поезда: " << train_id << "\n";
    file << "ID вагона: " << car_id << "\n";
    file << "Откуда: " << from_station << "\n";
    file << "Куда: " << to_station << "\n";
    file << std::fixed << std::setprecision(2);
    file << "Цена: " << price << " EUR\n";
    file << "=============================\n";

    file.close();
    std::cout << "Билет успешно сохранён в файл: " << filename << std::endl;
}

void User::buy_ticket(const Train& train, const std::string& from, const std::string& to, int car_id) {
    try {
        // Вычисление стоимости билета
        double price = train.calculate_price(from, to, car_id);
        // Добавление билета в список пользователя
        Ticket ticket = {train.get_id(), car_id, from, to, price, this};
        tickets.push_back(ticket);
        std::cout << "Билет куплен успешно!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

void User::show_tickets() const {
    std::cout << "Билеты пользователя " << passport.surname << ":" << std::endl;
    for (const auto& ticket : tickets) {
        ticket.print_ticket();
    }
}

void User::add_ticket(const Ticket& ticket) {
    tickets.push_back(ticket);
}
