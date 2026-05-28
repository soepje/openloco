#include "train_manager.h"
#include "openloco.h"

Train* TrainManager::AddTrain(Depot* depot, SavegameTrain* savegame_train) {
    if (field_4 > 3 || field_6 > 2) {
        return nullptr;
    }

    for (size_t i = 0; i < 4; i++) {
        if (!trains[i]) {
            if (!savegame_train) {
                uint32_t engine_resource_id =  0x1804 + (std::rand() % 3) * 2;
                Train* train = new Train(engine_resource_id, 0, false, false);
                trains[i] = train;




            } else {

            }

            break;
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
