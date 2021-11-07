// dsk

#include "game_object.hpp"

namespace dsk
{
    glm::mat4 DskGameObject::Transform3dComponent::mat4()
    {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);

        return glm::mat4
        {
            {
                scale.x * (c1 * c3 + s1 * s2 * s3),
                scale.x * (c2 * s3),
                scale.x * (c1 * s2 * s3 - c3 * s1),
                0.0f
            },
            {
                scale.y * (c3 * s1 * s2 - c1 * s3),
                scale.y * (c2 * c3),
                scale.y * (c1 * c3 * s2 + s1 * s3),
                0.0f
            },
            {
                scale.z * (c2 * s1),
                scale.z * (-s2),
                scale.z * (c1 * c2),
                0.0f
            },
            {
                translation.x,
                translation.y,
                translation.z,
                1.0f
            }
        };
    }

    glm::mat3 DskGameObject::Transform3dComponent::normalMatrix()
    {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        const glm::vec3 invScale = 1.0f / scale;

        return glm::mat3
        {
            {
                invScale.x * (c1 * c3 + s1 * s2 * s3),
                invScale.x * (c2 * s3),
                invScale.x * (c1 * s2 * s3 - c3 * s1)
            },
            {
                invScale.y * (c3 * s1 * s2 - c1 * s3),
                invScale.y * (c2 * c3),
                invScale.y * (c1 * c3 * s2 + s1 * s3)
            },
            {
                invScale.z * (c2 * s1),
                invScale.z * (-s2),
                invScale.z * (c1 * c2)
            }
        };
    }

    DskGameObject &DskGameObject::setModel(DskDevice &dskDevice, std::string filename)
    {
        std::shared_ptr<DskModel> _model;
        _model = DskModel::createModelFromFile(dskDevice, filename);
        
        model = _model;

        return *this;
    }

    DskGameObject &DskGameObject::setTexture
    (
        DskDevice &dskDevice,
        std::string filename,
        DskTexture textures[])
    {
        DskTexture texture;
        texture.loadImage(filename, dskDevice);

        textures[getId()] = texture;

        return *this;
    }

    DskGameObject &DskGameObject::setTranslation(glm::vec3 translation)
    {
        transform3d.translation = translation;
        
        return *this;
    }

    DskGameObject &DskGameObject::setScale(glm::vec3 scale)
    {
        transform3d.scale = scale;
        
        return *this;
    }

    DskGameObject &DskGameObject::setRotation(glm::vec3 rotation)
    {
        transform3d.rotation = rotation;
        
        return *this;
    }

    DskGameObject &DskGameObject::build
    (
        std::vector<DskGameObject> *gameObjects
    )
    {
        gameObjects->push_back(std::move(*this));
        
        return *this;
    }
}
