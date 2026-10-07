#include "train_manager.h"
#include "openloco.h"
#include "train_car.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>

TrainManager::TrainManager() {
    num_trains = 0;
    field_6 = 0;
    for (size_t i = 0; i < 4; i++) {
        train_cars[i] = nullptr;
    }
    for (size_t i = 0; i < 16; i++) {
        train_cars[i] = nullptr;
    }
}

Train* TrainManager::AddTrain(Depot* depot, SavegameTrain* savegame_train) {
    if (num_trains > 3 || field_6 > 2) {
        return nullptr;
    }

    char name[12];

    for (size_t i = 0; i < 4; i++) {
        if (!trains[i]) {
            if (!savegame_train) {
                uint32_t engine_resource_id =  0x1804 + (std::rand() % 3) * 2;
                Train* train = new Train(engine_resource_id, 0, false, false);
                trains[i] = train;

                if (train->cars[0] && train->cars[0]->ok) {
                    snprintf(name, 10, "%s %lu", "TODO", train->field_7a);
                    train->cars[0]->SetName(name);
                    for (int n = rand() % 5; n > 0; n--) {
                        int type = rand() % 3;
                        if (type == 0) {
                            int resource_id = (rand() % 3) * 2 + 6246;
                            train->AddCar(resource_id, TrainCarType::PASSENGER, false);
                        } else if (type == 1) {
                            int resource_id = (rand() % 2) * 2 + 6252;
                            train->AddCar(resource_id, TrainCarType::FREIGHT, false);
                        } else if (type == 2) {
                            train->AddCar(6256, TrainCarType::MAIL, false);
                        }
                    }
                } else {
                    delete train;
                    trains[i] = nullptr;
                    return nullptr;
                }
            } else {
                Train* train = new Train(savegame_train->resource_ids[0], 0, false, false);
                trains[i] = train;
                if (train->cars[0] && train->cars[0]->ok) {
                    snprintf(name, 10, "%s %lu", "TODO", train->field_7a);
                    train->cars[0]->SetName(name);
                    train->SetVisible(false);
                    for (size_t j = 0; j < 4; j++) {
                        if (savegame_train->resource_ids[j+1]) {
                            TrainCarType train_car_type = TrainCar::GetTrainCarType(savegame_train->resource_ids[0]);
                            train->AddCar(savegame_train->resource_ids[j+1], train_car_type, false);
                            train->SetVisible(false);
                        }
                    }
                }
            }

            num_trains++;
            field_6++;
            trains[i]->ExitDepot(depot, true);
            return trains[i];
        }
    }

    return nullptr;
}

void TrainManager::Update() {
    if (num_trains != 0 && (GAME_STATE == 3 || GAME_STATE == 9)) {
        for (auto train : trains) {
            if (train) {

            }
        }
    }
}

void TrainManager::DetectCollisions(Train* train) {
    // TODO
}

void TrainManager::Draw(Rect r, int16_t z) {
    for (size_t i = 0; i < 16; i++) {
        TrainCar* car = train_cars[i];
        if (!car) {
            break;
        }

        if (car->ShouldDraw(r.left, r.top, z)) {
            if(car->Draw(r)) {
                // TODO
            }
        }

        // TODO
    }
}

void TrainManager::UpdateTrainCars() {
    size_t n = 0;

    for (size_t i = 0; i < 4; i++) {
        Train* train = trains[i];
        if (train) {
            for (size_t j = 0; j <= train->last_car; j++) {
                TrainCar* train_car = train->cars[j];
                // TODO restore
                // if (train_car->depot_state != 2 && train_car->depot_state != 5 && train_car->tunnel_state != 2) {
                    train_cars[n] = train_car;
                    n++;
                // }
            }
        }
    }

    // lazy
    std::sort(train_cars, train_cars + n, [](const TrainCar* a, const TrainCar* b) { return a->bogie_front->y < b->bogie_front->y; });
}

void TrainManager::ClearTrainCars() {
    for (size_t i = 0; i < 16; i++) {
        train_cars[i] = nullptr;
    }
}
