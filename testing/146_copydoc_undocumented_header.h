// objective: test that \copydoc copies the documentation of a function declared in a header without a \file block
// check: 146__copydoc__undocumented__header_8h.xml

/**
 * @brief Opens the device.
 * @param id The device id.
 * @return Zero on success.
 */
int dev_open(int id);

/** @copydoc dev_open */
int dev_reopen(int id);
