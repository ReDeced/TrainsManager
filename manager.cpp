#include <fstream>
#include <iostream>
#include "manager.h"
#include "users.h"
#include "trains.h"
#include "json/single_include/nlohmann/json.hpp"

using json = nlohmann::json;

void Manager::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "❌ Не удалось открыть файл: " << filename << std::endl;
        return;
    }

    json data;
    file >> data;

    // --- Станции ---
    stations.clear();
    for (auto& s : data["stations"]) {
        stations.push_back({s["name"], {s["coords"][0], s["coords"][1]}});
    }

    // --- Типы поездов ---
    train_types.clear();
    for (auto& t : data["train_types"]) {
        train_types.push_back({t["name"], t["price_coef"]});
    }

    // --- Вагоны ---
    cars.clear();
    for (auto& c : data["cars"]) {
        cars.push_back({c["id"], c["price_coef"], c["seats_count"], c["seats_free"]});
    }

    // --- Поезда ---
    trains.clear();
    for (auto& t : data["trains"]) {
        auto type_it = std::find_if(train_types.begin(), train_types.end(),
                                    [&](const TrainType& tt){ return tt.name == t["train_type"]; });

        if (type_it != train_types.end()) {
            // Сначала создаём маршрут
            Route route;
            for (auto& sname : t["route"]) {
                auto it = std::find_if(stations.begin(), stations.end(),
                                       [&](const Station& s){ return s.name == sname; });
                if (it != stations.end())
                    route.add_station(*it);
            }

            // Создаём поезд с уже готовым маршрутом
            Train train(t["id"], *type_it, route, t["base_price"]);

            // Загружаем даты отправления
            for (auto& date : t["departure_dates"])
                train.add_departure_date(date);

            // Загружаем вагоны
            for (auto& cid : t["cars"]) {
                auto car_it = std::find_if(cars.begin(), cars.end(),
                                           [&](const Car& c){ return c.get_id() == cid; });
                if (car_it != cars.end())
                    train.add_car(*car_it);
            }

            trains.push_back(train);
        }
    }

    // --- Пользователи ---
    users.clear();
    for (auto& u : data["users"]) {
        Passport p{u["passport"]["name"], u["passport"]["surname"], u["passport"]["passport_id"].get<u_int64_t>(), u["passport"]["birth_date"]};
        users.push_back(User(u["id"], p));
    }

    // --- Билеты ---
    for (size_t i = 0; i < data["users"].size(); ++i) {
        auto& user = users[i];
        auto& ujson = data["users"][i];
        if (ujson.contains("tickets")) {
            for (auto& t : ujson["tickets"]) {
                Ticket ticket{t["train_id"], t["car_id"], t["from_station"], t["to_station"], t["price"], &user};
                user.add_ticket(ticket);
            }
        }
    }
}

void Manager::save_to_file(const std::string& filename) const {
    json data;

    // --- Станции ---
    for (auto& s : stations) {
        data["stations"].push_back({
            {"name", s.name},
            {"coords", {s.position.x, s.position.y}}
        });
    }

    // --- Типы поездов ---
    for (auto& t : train_types) {
        data["train_types"].push_back({
            {"name", t.name},
            {"price_coef", t.price_coef}
        });
    }

    // --- Вагоны ---
    for (auto& c : cars) {
        data["cars"].push_back({
            {"id", c.get_id()},
            {"price_coef", c.get_price_coef()},
            {"seats_count", c.get_seats_count()},
            {"seats_free", c.get_seats_free()}
        });
    }

    // --- Поезда ---
    for (auto& tr : trains) {
        json train_json;
        train_json["id"] = tr.get_id();
        train_json["train_type"] = tr.get_type().name;
        train_json["base_price"] = tr.get_base_price();
        train_json["departure_dates"] = tr.get_departure_dates();

        // --- Маршрут как список станций внутри поезда ---
        json route_json = json::array();
        for (auto& s : tr.get_route().get_stations())
            route_json.push_back(s.name);
        train_json["route"] = route_json;

        // --- Вагоны ---
        json cars_json = json::array();
        for (auto& c : tr.get_cars())
            cars_json.push_back(c.get_id());
        train_json["cars"] = cars_json;

        data["trains"].push_back(train_json);
    }

    // --- Пользователи ---
    for (auto& u : users) {
        json user_json;
        user_json["id"] = u.get_id();
        user_json["passport"] = {
            {"name", u.get_passport().name},
            {"surname", u.get_passport().surname},
            {"passport_id", u.get_passport().passport_id},
            {"birth_date", u.get_passport().birth_date}
        };

        // --- Билеты пользователя ---
        json tickets_json = json::array();
        for (auto& t : u.get_tickets()) {
            tickets_json.push_back({
                {"train_id", t.train_id},
                {"car_id", t.car_id},
                {"from_station", t.from_station},
                {"to_station", t.to_station},
                {"price", t.price}
            });
        }
        user_json["tickets"] = tickets_json;

        data["users"].push_back(user_json);
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "❌ Не удалось открыть файл для сохранения: " << filename << std::endl;
        return;
    }

    file << std::setw(4) << data << std::endl; // Красивое форматирование
}

void Manager::load_from_file() {
    load_from_file(filename);
}

void Manager::save_to_file() const {
    save_to_file(filename);
}

Manager::Manager(const std::string& filename) : filename(filename) {
    load_from_file(filename);
}

Manager::~Manager() {
    save_to_file();
}


User& Manager::register_user() {
    Passport p;
    std::cout << "Введите имя: ";
    std::getline(std::cin, p.name);
    std::cout << "Введите фамилию: ";
    std::getline(std::cin, p.surname);
    std::cout << "Введите номер паспорта (10 цифр): ";
    std::cin >> p.passport_id;
    std::cin.ignore();
    std::cout << "Введите дату рождения (ГГГГ-ММ-ДД): ";
    std::getline(std::cin, p.birth_date);

    int new_id = users.empty() ? 1 : users.back().get_id() + 1;
    users.emplace_back(new_id, p);

    std::cout << "Пользователь зарегистрирован! ID: " << new_id << "\n";
    return users.back();
}

User* Manager::get_user(u_int64_t passport_id) {
    for (auto& u : users) {
        if (u.get_passport().passport_id == passport_id) {
            std::cout << "Добро пожаловать, " << u.get_passport().name << "!\n";
            return &u;
        }
    }
    std::cout << "Пользователь с таким паспортом не найден.\n";
    return nullptr;
}
