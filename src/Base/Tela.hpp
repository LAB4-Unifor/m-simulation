#pragma once
#include <stack>
#include <memory>

class Tela;

class CoordenadorTelas
{
    public:
        void HandleInput();
        void Update();
        void Draw();
        
        ~CoordenadorTelas();
        
        Tela *PeekTela();
        void PushTela(Tela *tela);
        void PopTela();
        void ChangeTela(Tela* tela);
        
        bool ShouldQuit() {return m_shouldQuit;}
        
        bool m_shouldQuit = false;
    private:
        std::stack<Tela*> telas;
};

class Tela
{
    public:
        Tela() = default;
        virtual ~Tela() = default;
        virtual void Init() = 0;
        virtual void HandleInput() = 0;
        virtual void Update() = 0;
        virtual void Draw() = 0;
        
        virtual void Pause() = 0;
        virtual void Resume() = 0;
        
        virtual void OnExit() = 0;
        virtual void OnEnter() = 0;
        
        virtual void CleanUp() = 0;
        
    private:
        std::shared_ptr<CoordenadorTelas> m_cTelas = nullptr;
};
