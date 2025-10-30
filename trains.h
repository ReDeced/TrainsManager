#ifndef TRAINS_H
#define TRAINS_H

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

struct Coordinates {
    double x;
    double y;
    
    double distanceTo(const Coordinates& other) const {
        return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2));
    }
};

struct Station {
    std::string name;
    Coordinates position;
};

class Route {
protected:
    std::vector<Station> stations;

public:
    void add_station(const Station& station);
    double get_distance_between(const std::string& from_name, const std::string& to_name) const;
    const std::vector<Station>& get_stations() const { return stations; }
    void print_route() const;
};

struct TrainType {
    std::string name;
    double price_coef;
};

class Car {
protected:
    int id;
    double price_coef;
    int seats_count;
    int seats_free;

public:
    Car(int id, double price_coef, int seats);
    Car(int id, double price_coef, int seats, int seats_free);
    int get_id() const { return id; }
    double get_price_coef() const;
    double get_seats_count() const;
    double get_seats_free() const;
    friend class Train;
};

class Train {
protected:
    int id;
    TrainType type;
    Route route;
    double base_price_per_km;
    std::vector<std::string> departure_dates;
    std::vector<Car> cars;

public:
    Train(int id, TrainType type, Route route, double price_coef);
    void add_departure_date(std::string date);
    void add_car(const Car& car);
    double calculate_price(const std::string& from, const std::string& to, int car_id) const;
    Route& get_route() { return route; }
    const Route& get_route() const { return route; }
    int get_id() const;
    const TrainType& get_type() const { return type; }
    double get_base_price() const { return base_price_per_km; }
    const std::vector<std::string>& get_departure_dates() const { return departure_dates; }
    const std::vector<Car>& get_cars() const { return cars; }
};

#endif // TRAINS_H
