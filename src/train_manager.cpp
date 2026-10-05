#include "train_manager.h"
#include "openloco.h"
#include "train_car.h"
#include <cstdio>
#include <cstdlib>

Train* TrainManager::AddTrain(Depot* depot, SavegameTrain* savegame_train) {
    if (field_4 > 3 || field_6 > 2) {
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

            field_4++;
            field_6++;
            trains[i]->ExitDepot(depot, true);
            return trains[i];
        }
    }

    return nullptr;
}

void TrainManager::Update() {
    if (field_4 != 0 && (GAME_STATE == 3 || GAME_STATE == 9)) {
        for (auto train : trains) {
            if (train) {

            }
        }
    }
}

void TrainManager::DetectCollisions(Train* train) {
    // TODO
}
