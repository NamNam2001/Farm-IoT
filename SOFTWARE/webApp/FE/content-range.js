module.exports = (req, res, next) => {
    res.header('Access-Control-Expose-Headers', 'Content-Range');
    res.header('Content-Range', `posts 0-10/20; comments 0-99/200; profile 0-0/1; users 0-199/300; products 0-149/200;`);
    next();
}
