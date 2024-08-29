module.exports = (req, res, next) => {
    res.header('Access-Control-Expose-Headers', 'Content-Range');
    res.header('Content-Range', `devices 0-10/20 ; dashboards 0-2/4;`);
    next();
}
