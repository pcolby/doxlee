/*!
 * \file
 * Example C++ file
 *
 * This file includes a range of C++ samples for demonstrating Doxygen XML output. The focus here is
 * on bredth of features covered, rather than depth.
 */

/*!
 * The abc namespace encapsulates everything in this file.
 */
namespace abc {

/*!
 * The def namespace encapsulates everything in this file, except the abc namespace.
 */
namespace def {

/*!
 * The Foo class.
 *
 * This class is pretty simple.
 */
class Foo {
public:
    int i; ///< A public class member.
    int j; ///< Another public class member.
    int k; ///< Yet another public class member.

    /*!
     * Constructs a Foo object.
     *
     * \todo Make this constructor do more.
     */
    Foo() : i(0) { }

    /*!
     * Constructs a Foo object.
     *
     * \overload
     */
    explicit Foo(const int i = 123) : i(i) { }

    /*!
     * Do something.
     *
     * Anyone may call this funtion.
     *
     * \return \c true, pretty much always.
     */
    bool doSomething() { return true; }

    /*!
     * Do something virtually.
     *
     * Derived classes *must* override this function to do something.
     */
    void doSomethingVirtually() = 0;

protected:
    int l; ///< A protected class member.
    int m; ///< Another protected class member.
    int n; ///< Yet another protected class member.

    /*!
     * Do something overridable.
     *
     * Derived clases may override to do their own thing.
     *
     * \return some integer
     */
    virtual int doSomethingOverridable() { return  123; }

private:
    int o; ///< A private class member.
    int p; ///< Another private class member.
    int q; ///< Yet another private class member.

    /*!
     * \internal
     *
     * Does something privately.
     *
     * \note This is not part of any public interface.
     *
     * \return
     */
    static int doSomethingPrivately() { return 456; }

};

/*!
 * The Bar class extends Foo to do bar.
 */
class Bar : public Foo {

public:
    /*!
     * Do something virtually.
     *
     * This implementation does nada (other than demonstrating providing an implelemtation for a
     * pure virtual function).
     */
    void doSomethingVirtually() { }

protected:
    int doSomethingOverridable() override { return  123; }

};

/*!
 * The Bar class extends Foo to do baz.
 */
class Baz : private Foo {

};

} // namesapce def

} // namespace abc

