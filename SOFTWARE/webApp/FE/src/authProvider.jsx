import { AuthProvider } from 'react-admin';

export default {
    // dipanggil ketika user mencoba untuk login
    login: ({ username }) => {
      localStorage.setItem("username", username);
      // berhasil mencocokkan username dan password
      return Promise.resolve();
    },
    // dipanggil ketika user menekan tomboll logout
    logout: () => {
      localStorage.removeItem("username");
      return Promise.resolve();
    },
    // dipanggil ketika API mengembalikan error
    checkError: ({ status }) => {
      if (status === 401 || status === 403) {
        localStorage.removeItem("username");
        return Promise.reject();
      }
      return Promise.resolve();
    },
    // dipanggil ketika user bernavigasi ke tempat baru, cek authentikasi lagi
    checkAuth: () => {
      return localStorage.getItem("username")
        ? Promise.resolve()
        : Promise.reject();
    },
    // dipanggil ketika user bernavigasi ke tempat baru, cek permission/role lagi
    getPermissions: () => Promise.resolve()
  };
  