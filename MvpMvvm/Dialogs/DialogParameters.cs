using MvpMvvm.Common;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MvpMvvm.Dialogs
{
    public class DialogParameters : ParametersBase, IDialogParameters
    {
        /// <summary>
        /// Initializes a new instance of <see cref="DialogParameters"/>.
        /// </summary>
        public DialogParameters()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="DialogParameters"/> based on a specified query string.
        /// </summary>
        /// <param name="query">A uri query string</param>
        public DialogParameters(string query)
            : base(query)
        {
        }
    }
}
