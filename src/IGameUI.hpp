#ifndef I_GAME_UI_HPP
#define I_GAME_UI_HPP

#include <string>
#include <vector>
#include <utility> // For std::pair

// Forward-declare the FocusState enum to avoid including GameUI.hpp
enum class FocusState { BOARD, MENU };

class IGameUI {
public:
    virtual ~IGameUI() = default;

    // --- Core UI Functions ---
    virtual void displayBoard() const noexcept = 0;
    virtual int getPressedKey() const noexcept = 0;
    virtual void displayMessage(const std::string& message) const noexcept = 0;
    virtual void flashScreen() const noexcept = 0;

    // --- State Setters (called by GameController) ---
    virtual void setFocus(FocusState new_focus) noexcept = 0;
    virtual void setCursorPosition(int row, int col) noexcept = 0;
    virtual void setSelectedMenuItem(int item) noexcept = 0;
    virtual void setErrors(const std::vector<std::pair<int, int>>& errors) noexcept = 0;

    // --- State Getters (needed by GameController) ---
    virtual std::pair<int, int> getCursorPosition() const noexcept = 0;
    virtual FocusState getFocus() const noexcept = 0;
    virtual const std::vector<std::string>& getMenuItems() const noexcept = 0;
    virtual int getSelectedMenuItem() const noexcept = 0;
};

#endif // I_GAME_UI_HPP