// objective: check that modifiers are correctly parsed with trailing return types
// check: class_c.xml

/** @brief A structure. */
class C {
public:
    /** @brief Pure. */
    virtual auto f_pure() -> int = 0;
    /** @brief Override. */
    auto f_override() -> int override;
    /** @brief Final. */
    virtual auto f_final() -> int final;
    /** @brief Final and override. */
    virtual auto f_override_final() -> int override final;

    /** @brief Deleted copy operator. */
    auto operator=(C const&) -> C& = delete;
    /** @brief Defaulted move operator. */
    auto operator=(C&&) -> C& = default;
};
