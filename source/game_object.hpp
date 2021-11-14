// dsk

#pragma once

#include "model.hpp"
#include "texture.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include <memory>

namespace dsk
{
    class DskGameObject
    {
        struct Transform3dComponent
        {
            glm::vec3 translation {};
            glm::vec3 scale {1.0f};
            glm::vec3 rotation {};

            glm::mat4 mat4();
            glm::mat3 normalMatrix();
        };

        public:
            using id_t = unsigned int;

            static DskGameObject createGameObject()
            {
                static id_t currentId = 0;

                return DskGameObject {currentId++};
            }

            id_t getId() { return id; }

            DskGameObject &setModel
            (
                std::string filename,
                DskDevice &dskDevice
            );
            DskGameObject &setTexture
            (
                std::string filename,
                DskTexture textures[],
                DskDevice &dskDevice
            );
            DskGameObject &setTag         (std::string name       );
            DskGameObject &setTranslation (glm::vec3   translation);
            DskGameObject &setScale       (glm::vec3   scale      );
            DskGameObject &setRotation    (glm::vec3   rotation   );
            DskGameObject &build          (std::vector<DskGameObject> *gameObjects);

            inline std::string getTag()         { return tag;                   }
            inline glm::vec3   getTranslation() { return transform.translation; }
            inline glm::vec3   getScale()       { return transform.scale;       }
            inline glm::vec3   getRotation()    { return transform.rotation;    }

            std::shared_ptr<DskModel> model {};
            glm::vec3 color {};
            Transform3dComponent transform {};
            std::string tag;

        private:
            DskGameObject(id_t objId): id{objId} {}

            id_t id;
    };
}
