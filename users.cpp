#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include "trains.h"
#include "users.h"
#include <filesystem>

void Ticket::print_ticket() const {
    std::cout << "Билет на поезд: " << train_id << std::endl;
    std::cout << "Маршрут: " << from_station << " -> " << to_station << std::endl;
    std::cout << "Класс вагона: " << car_id << std::endl;
    std::cout << "Цена: " << price << " руб." << std::endl;
}

void Ticket::save_to_file() const {
    try {
        std::filesystem::path dir("tickets");
        if (!std::filesystem::exists(dir)) {
            if (!std::filesystem::create_directory(dir)) {
                throw std::ios_base::failure("Не удалось создать папку tickets");
            }
        }

        std::string filename = "tickets/ticket_" + std::to_string(train_id) + "_" + owner->get_passport().name + ".txt";

        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::ios_base::failure("Не удалось открыть файл " + filename + " для записи");
        }

        file << "====== Билет на поезд ======\n";
        file << "Владелец: " << owner->get_passport().name << "\n";
        file << "ID поезда: " << train_id << "\n";
        file << "ID вагона: " << car_id << "\n";
        file << "Откуда: " << from_station << "\n";
        file << "Куда: " << to_station << "\n";
        file << std::fixed << std::setprecision(2);
        file << "Цена: " << price << " EUR\n";
        file << "=============================\n";

        if (file.fail()) {
            throw std::ios_base::failure("Ошибка при записи данных в файл " + filename);
        }

        file.close();
        std::cout << "Билет успешно сохранён в файл: " << filename << std::endl;
    }
    catch (const std::ios_base::failure& e) {
        std::cerr << "Файловая ошибка: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Произошла ошибка: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Неизвестная ошибка при сохранении билета." << std::endl;
    }
}

void User::show_tickets() const {
    std::cout << "Билеты пользователя " << passport.surname << ":" << std::endl;
    for (const auto& ticket : tickets) {
        ticket.print_ticket();
    }
}

void User::add_ticket(Ticket& ticket) {
    ticket.owner = this;
    tickets.push_back(ticket);
}
