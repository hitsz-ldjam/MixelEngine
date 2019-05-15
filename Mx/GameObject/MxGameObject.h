#pragma once

#ifndef MX_GAME_OBJECT_H_
#define MX_GAME_OBJECT_H_

#include <algorithm>
#include <set>

#include "../Object/MxObject.h"
#include "../Component/MxComponent.h"
#include "../Definitions/MxDefinitions.h"
#include "../Component/Transform/MxTransform.h"

#ifdef _RESOURCE_MANAGER_ENABLED
#include "../Resource/MxResourceBase.h"
#endif

namespace Mix {
    class Component;
    class Transform;

    class GameObject : public Object {
    MX_DECLARE_RTTI
    MX_DECLARE_CLASS_FACTORY

    public:
        GameObject(const std::string& _name = "",
                   Tag _tag = "",
                   const LayerIndex _layer = 0,
                   const bool _isStatic = false);

        GameObject(const GameObject& _obj) = delete;

        GameObject(GameObject&& _obj);

        GameObject& operator=(const GameObject& _obj) = delete;

        GameObject& operator=(GameObject&& _obj);

        virtual ~GameObject();

        /**
         *  @brief Add a Component of type _Ty to this GameObject, using default constructor to create the Component
         *  @return The pointer to the Component
         */
        template <typename _Ty>
        _Ty* addComponent();

        /**
         *  @brief Add Component _comp to this GameObject
         *  @return The pointer to the Component
         */
        Component* addComponent(Component* _comp);

        /**
         *  @brief Construct a Component of type _Ty and add it to this GameObject
         *  @return The pointer to the Component
         */
        template <typename _Ty, typename... Args>
        _Ty* addComponent(Args&& ..._args);

        /**
         *  @brief Get the pointer to the Component of type _Ty that attached to this GameObject
         *  @return The pointer to the Component, return nullptr if not found
         */
        template <typename T>
        T* getComponent();

        /**
         *  @brief Get all Components of type _Ty that attached to this GameObject
         *  @return A vector containing pointers of Components that been found
         */
        template <typename _Ty>
        std::vector<_Ty*> getComponents();

        /**
         *  @brief Get Component of type _Ty in children of this GameObject using DFS
         */
        template <typename _Ty>
        _Ty* getComponentInChildren();

        /**
         *  @brief Remove Component that _comp points to,
         * do nothing if _comp isn't attached to this GameObject
         */
        void removeComponent(Component* _comp);

        /**
         *  @brief Add _obj as a child of this GameObject
         */
        void addChild(GameObject* _obj);

        /**
         *  @brief Remove a child that _obj points to from this GameObject
         */
        void removeChild(GameObject* _obj);

        bool activeInHierarchy() const {
            return mActiveInHierarchy;
        }

        /**
         *  @brief Check if this GameObject is active
         */
        bool activeSelf() const {
            return mActiveSelf;
        }

        /**
         *  @brief Activates/Deactivates the GameObject, depending on the given true or false value.
         */
        void setActive(const bool _active);

        /**
         *  @note There is not getter of this field because altering this will affect its physics.
         */
        bool isStatic() const {
            return mIsStatic;
        }

        /**
         *  @brief Get the index of the layer that this GameObject belongs to
         */
        LayerIndex getLayer() const {
            return mLayer;
        }

        /**
         *  @brief Set the index of the layer that this GameObject belongs to
         */
        void setLayer(const LayerIndex _index) {
            mLayer = _index;
        }

        /**
         *  @brief Get the tag attached to this GameObject
         */
        const Tag& getTag() const {
            return mTag;
        }

        /**
         *  @brief Set a tag to this GameObject
         */
        void setTag(const Tag& _tag) {
            mTag = _tag;
        }

        Transform& transform() {
            return *mTransform;
        }

#ifdef _RESOURCE_MANAGER_ENABLED
        void SetModelRef(Resource::ResourceRef _ref) {
            mModeRef = _ref;
        }

        Resource::ResourceRef GetModelRef() const {
            return mModeRef.value_or(nullptr);
        }
#endif

    protected:
        GameObject* mParent;
        std::set<GameObject*> mChildren;
        std::set<Component*> mComponents;

        bool mActiveInHierarchy, mActiveSelf, mIsStatic;
        LayerIndex mLayer;
        Tag mTag;
        Transform* mTransform;
        // todo: set scene
        // Scene* scene;

#ifdef _RESOURCE_MANAGER_ENABLED
        std::optional<Resource::ResourceRef> mModeRef;
#endif

        // ----- static variables and functions -----

    public:
        /**
         *  @brief Get a GameObject named _name
         *  @return The Pointer to the GameObject, return nullptr if not found
         */
        static GameObject* Find(const std::string& _name);

        /**
         *  @brief Get all GameObjects with tag _tag
         *  @return A vector containing pointers to GameObjects
         */
        static std::vector<GameObject*> FindGameObjectsWithTag(const Tag& _tag);

        /**
         *  @brief Get a GameObject with tag _tag
         *  @return The pointer to the first GameObject been found, return nullptr if not found
         */
        static GameObject* FindGameObjectWithTag(const Tag& _tag);

    private:
        static std::vector<GameObject*> sGameObjList;

        /**
         *  @brief Add GameObject _obj points to to global GameObject list
         */
        static void AddGameObject(GameObject* _obj) {
            sGameObjList.push_back(_obj);
        }

        /**
         *  @brief Remove the GameObject _obj points to fom global GameObject list
         */
        static void RemoveGameObject(GameObject* _obj) {
            const auto it = std::find(sGameObjList.begin(), sGameObjList.end(), _obj);
            if(it == sGameObjList.end())
                return;
            sGameObjList.erase(it);
        }
    };

    // ----- template function implementations -----

    template <typename _Ty>
    inline _Ty* GameObject::addComponent() {
        // if type _Ty isn't derived from Component
        _Ty* t = reinterpret_cast<_Ty*>(1);
        if(!dynamic_cast<Component*>(t))
            throw ComponentCastingError();

        t = new _Ty();
        t->setGameObject(this);
        mComponents.insert(t);
        return t;
    }

    template <typename T, typename ...Args>
    inline T* GameObject::addComponent(Args&& ..._args) {
        // if type _Ty isn't derived from Component
        T* t = reinterpret_cast<T*>(1);
        if(!dynamic_cast<Component*>(t))
            throw ComponentCastingError();

        t = new T(std::forward<Args>(_args)...);
        t->setGameObject(this);
        mComponents.insert(t);
        return t;
    }

    template <typename T>
    inline T* GameObject::getComponent() {
        T* result;

        for(auto comp : mComponents) {
            if((result = dynamic_cast<T*>(comp)))
                return result;
        }

        return nullptr;
    }

    template <typename T>
    inline std::vector<T*> GameObject::getComponents() {
        std::vector<T*> results;
        T* result;

        for(auto comp : mComponents) {
            if((result = dynamic_cast<T*>(comp)))
                results.push_back(result);
        }

        return results;
    }

    template <typename T>
    inline T* GameObject::getComponentInChildren() {
        T* ptr = getComponent<T>();

        if(!ptr) {
            for(auto child : mChildren) {
                if((ptr = child->getComponentInChildren<T>()))
                    return ptr;
            }
        }

        return nullptr;
    }
}

#endif