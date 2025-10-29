#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "trains.cpp"

void initialize_railway_system(std::vector<Station>& stations, 
                              std::vector<TrainType>& train_types,
                              std::vector<Route>& routes,
                              std::vector<Car>& cars,
                              std::vector<Train>& trains) {
    
    // Инициализация станций
    stations = {
        {"Хогвартс", {54.1930, 37.6173}},
        {"Тридевятое Царство", {59.9311, 30.3609}},
        {"Тридесятое Государство", {55.7558, 37.6173}},
        {"Звездный Городок", {55.8722, 38.1131}},
        {"Атлантида", {43.5853, 39.7233}},
        {"Шангри-Ла", {43.0167, 131.9000}},
        {"Китеж-град", {56.3269, 44.0065}}
    };

    // Инициализация типов поездов
    train_types = {
        {"Магический экспресс", 1.5},
        {"Космический курьер", 2.0},
        {"Сказочный поезд", 1.2},
        {"Обычный маршрут", 1.0}
    };

    // Инициализация вагонов
    cars = {
        {1, 2.5, 20},  // Люкс
        {2, 1.8, 36},  // Купе
        {3, 1.2, 54},  // Плацкарт
        {4, 1.0, 80},  // Общий
        {5, 3.0, 16},  // Магический
        {6, 2.8, 24}   // Космический
    };

    // Создание маршрутов
    Route magical_route;
    magical_route.add_station(stations[0]); // Хогвартс
    magical_route.add_station(stations[6]); // Китеж-град
    magical_route.add_station(stations[1]); // Тридевятое Царство
    magical_route.add_station(stations[2]); // Тридесятое Государство

    Route fantasy_route;
    fantasy_route.add_station(stations[3]); // Звездный Городок
    fantasy_route.add_station(stations[4]); // Атлантида
    fantasy_route.add_station(stations[5]); // Шангри-Ла
    fantasy_route.add_station(stations[2]); // Тридесятое Государство

    routes.push_back(magical_route);
    routes.push_back(fantasy_route);

    // Создание времени отправления
    std::string departure_dates[4] = {"2024-03-15", "2024-03-16", "2024-03-17", "2024-03-18"};

    // Создание поездов
    Train magical_train(1, train_types[0], magical_route, 0.5);
    magical_train.add_departure_date(departure_dates[0]);
    magical_train.add_departure_date(departure_dates[1]);
    magical_train.add_car(cars[4]); // Магический вагон
    magical_train.add_car(cars[0]); // Люкс

    Train fantasy_train(2, train_types[1], fantasy_route, 0.7);
    fantasy_train.add_departure_date(departure_dates[2]);
    fantasy_train.add_departure_date(departure_dates[3]);
    fantasy_train.add_car(cars[5]); // Космический вагон
    fantasy_train.add_car(cars[1]); // Купе

    trains.push_back(magical_train);
    trains.push_back(fantasy_train);
}
