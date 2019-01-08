// objective: allow linking to private virtual functions
// check: class_interface.xml
// config: EXTRACT_PRIV_VIRTUAL = YES

/** @brief An interface */
class Interface {
  public:
    /**
     * @brief Load things.
     *
     * Calls @ref doLoad().
     */
    void load();

  private:
    /**
     * @brief Pure virtual implementation for @ref load()
     *
     * Details.
     */
    virtual void doLoad() = 0;

    /**
     * @brief Non-pure virtual function
     *
     * Details.
     */
    virtual void doOtherStuff();

    /* Undocumented, should not appear in the docs */
    virtual void doSomethingUndocumented();

    /** @brief A non-virtual private function, not extracted */
    void someUtility();
};
