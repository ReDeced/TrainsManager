#include "trains.h"

void Route::add_station(const Station& station) {
    stations.push_back(station);
}

double Route::get_distance_between(const std::string& from_name, const std::string& to_name) const {
    Coordinates start_pos, end_pos;
    bool start_found = false, end_found = false;
    
    for (const auto& station : stations) {
        if (station.name == from_name) {
            start_pos = station.position;
            start_found = true;
        }
        if (station.name == to_name) {
            end_pos = station.position;
            end_found = true;
        }
    }
    
    if (!start_found || !end_found) {
        throw std::runtime_error("Ошибка: Станция не найдена!");
    }
    
    return start_pos.distanceTo(end_pos);
}

void Route::print_route() const {
    std::cout << "Маршрут: ";
    for (size_t i = 0; i < stations.size(); ++i) {
        std::cout << stations[i].name;
        if (i < stations.size() - 1) {
            std::cout << " -> ";
        }
    }
    std::cout << std::endl;
}

Car::Car(int id, double price_coef, int seats)
    : id(id), price_coef(price_coef), seats_count(seats), seats_free(seats) {}

double Car::get_price_coef() const { return price_coef; }
double Car::get_seats_free() const { return seats_free; }

Train::Train(int id, TrainType type, Route route, double price_coef)
    : id(id), route(route), type(type) {
    base_price_per_km = type.price_coef * price_coef;
}

void Train::add_departure_date(std::string date) {
    departure_dates.push_back(date);
}

void Train::add_car(const Car& car) {
    cars.push_back(car);
}

int Train::get_id() const {
    return id;
}

double Train::calculate_price(const std::string& from, const std::string& to, int car_id) const {
    double distance = route.get_distance_between(from, to);
    double car_coef = 0.0;

    for (const auto& c : cars) {
        if (c.id == car_id) {
            car_coef = c.price_coef;
            break;
        }
    }

    if (car_coef == 0.0) {
        throw std::runtime_error("Ошибка: Номер вагона не найден!");
    }
    
    return distance * base_price_per_km * car_coef;
}
