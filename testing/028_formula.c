// objective: test the \f$, \f[, \f], \f{, and \f} commands
// check: indexpage.xml

/** @mainpage
Here are some formulas:
-# The distance between \f$(x_1,y_1)\f$ and \f$(x_2,y_2)\f$ is
  \f$\sqrt{(x_2-x_1)^2+(y_2-y_1)^2}\f$.
-# Unnumbered formula:
   \f[ |I_2|=\left| \int_{0}^T \psi(t) \left\{ u(a,t)- \int_{\gamma(t)}^a
       \frac{d\theta}{k(\theta,t)} \int_{a}^\theta c(\xi)u_t(\xi,t)\,d\xi \right\} dt \right| \f]
-# Formula in different environment
  \f{eqnarray*}{ g &=& \frac{Gm_2}{r^2} \\
                   &=& \frac{(6.673 \times 10^{-11}\,\mbox{m}^3\,\mbox{kg}^{-1}\,
                       \mbox{s}^{-2})(5.9736 \times 10^{24}\,\mbox{kg})}{(6371.01\,\mbox{km})^2} \\
                       &=& 9.82066032\,\mbox{m/s}^2
 \f}
*/
