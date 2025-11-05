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
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
        return;
    }

    json data;
    file >> data;
std::cout << "Y";   
    // --- Станции ---
    stations.clear();
    for (auto& s : data["stations"]) {
        stations.push_back({s["name"], {s["coords"][0], s["coords"][1]}});
    }
std::cout << "Y";
    // --- Типы поездов ---
    train_types.clear();
    for (auto& t : data["train_types"]) {
        train_types.push_back({t["name"], t["price_coef"]});
    }
std::cout << "Y";
    // --- Вагоны ---
    std::vector<Car> _cars;
    _cars.clear();
    for (auto& c : data["cars"]) {
        _cars.push_back({c["id"], c["price_coef"], c["seats_count"]});
    }
std::cout << "Y";
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
            for (auto& c : t["cars"]) {
                Car car = { c["id"], c["price_coef"], c["seats"] };
                train.add_car(car);
            }

            trains.push_back(train);
        }
    }
std::cout << "Y";
    // --- Пользователи ---
    users.clear();
    for (auto& u : data["users"]) {
        Passport p{u["passport"]["name"], u["passport"]["surname"], u["passport"]["passport_id"].get<u_int64_t>(), u["passport"]["birth_date"]};
        users.push_back(User(u["id"], p));
    }
std::cout << "Y";
    // --- Билеты ---
    for (size_t i = 0; i < data["users"].size(); ++i) {
        auto& user = users[i];
        auto& ujson = data["users"][i];
        if (ujson.contains("tickets")) {
            for (auto& t : ujson["tickets"]) {
                for (auto& u : users) {
                    if (u.get_id() == ujson["id"])
                        buy_ticket(u, t["train_id"], t["car_id"], t["from"], t["to"]);
                }
            }
        }
    }
}

void Manager::save_to_file(const std::string& filename) const {
    try {
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

        // --- Поезда ---
        for (auto& tr : trains) {
            json train_json;
            train_json["id"] = tr.get_id();
            train_json["train_type"] = tr.get_type().name;
            train_json["base_price"] = tr.get_base_price();
            train_json["departure_dates"] = tr.get_departure_dates();

            // --- Маршрут ---
            json route_json = json::array();
            for (auto& s : tr.get_route().get_stations())
                route_json.push_back(s.name);
            train_json["route"] = route_json;

            // --- Вагоны ---
            json cars_json = json::array();
            for (auto& c : tr.get_cars()) {
                cars_json.push_back({
                    {"id", c.get_id()},
                    {"price_coef", c.get_price_coef()},
                    {"seats", c.get_seats_count()}
                });
            }
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

            // --- Билеты ---
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

        // --- Сохраняем в файл ---
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::ios_base::failure("Не удалось открыть файл для записи");
        }

        file << std::setw(4) << data << std::endl;

        if (file.fail()) {
            throw std::ios_base::failure("Ошибка при записи данных в файл");
        }

        std::cout << "Данные успешно сохранены в файл: " << filename << std::endl;
    }
    catch (const json::exception& e) {
        std::cerr << "Ошибка при работе с JSON: " << e.what() << std::endl;
    }
    catch (const std::ios_base::failure& e) {
        std::cerr << "Файловая ошибка: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Произошла ошибка: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Неизвестная ошибка при сохранении данных." << std::endl;
    }
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

void Manager::show_routes() const {
    for (auto& t : trains) {
        int seats_count = 0;
        for (auto& c : t.get_cars()) {
            seats_count += c.get_seats_free();
        }
        if (seats_count != 0) {
            t.print_route();
            std::cout << "\n";
        }
    }
}

Ticket& Manager::buy_ticket(User& user, int train_id, int car_id, const std::string& from, const std::string& to) {
    for (auto& train : trains) {
        if (train.get_id() == train_id) {
            const auto& stations = train.get_route().get_stations();
            auto it_from = std::find(stations.begin(), stations.end(), from);
            auto it_to = std::find(stations.begin(), stations.end(), to);

            if (it_from == stations.end() || it_to == stations.end()) {
                throw std::runtime_error("Invalid station names");
            }

            if (std::distance(stations.begin(), it_from) >= std::distance(stations.begin(), it_to)) {
                throw std::runtime_error("'from' station must be before 'to' station in the route");
            }

            double price = train.calculate_price(from, to, car_id);

            std::cout << "Стоимость билета: " << price << " руб." << std::endl;
            std::cout << "Подтвердить покупку? (д/н): ";

            std::string confirm;
            std::cin >> confirm;

            if (confirm != "д" && confirm != "Д") {
                std::cout << "Покупка отменена." << std::endl;
                throw std::runtime_error("Purchase canceled");
            }

            Ticket ticket = {train_id, car_id, from, to, price};
            user.add_ticket(ticket);
            std::cout << "Билет куплен успешно!" << std::endl;

            // возвращаем ссылку на только что добавленный билет
            return user.get_tickets().back();
        }
    }

    throw std::runtime_error("Train not found");
}
