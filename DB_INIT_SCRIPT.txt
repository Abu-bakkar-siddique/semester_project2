CREATE TABLE blockchain (
    id INTEGER PRIMARY KEY,
    prev_hash TEXT NOT NULL,
    hash TEXT NOT NULL,
    FOREIGN KEY(prev_hash) REFERENCES blocks(previous_block_hash)
    FOREIGN KEY (hash) REFERENCES blocks(block_hash)
)

CREATE TABLE blocks
(
    id INTEGER PRIMARY KEY,
    previous_block_hash TEXT,
    block_hash TEXT NOT NULL,
    transaction_id INTEGER NOT NULL UNIQUE  ,
    time_stamp DATETIME DEFAULT CURRENT_TIMESTAMP, approved INTEGER DEFAULT 0 CHECK (approved IN (0, 1)), 
    FOREIGN KEY (transaction_id) REFERENCES transactions(id)
)

CREATE TABLE miners (
    miner_id INTEGER,
    wallet_balance REAL NOT NULL,
    FOREIGN KEY (miner_id) REFERENCES nodes(id),
    FOREIGN KEY (wallet_balance) REFERENCES nodes(wallet_balance)
)

CREATE TABLE "node_approvals" (
    id INTEGER PRIMARY KEY,
    node_id INT,
    block_id INT,
    approve INT DEFAULT 0 CHECK (approve IN (0, 1)),
    FOREIGN KEY (node_id) REFERENCES nodes(id),
    FOREIGN KEY (block_id) REFERENCES blocks(id)
)

CREATE TABLE nodes
(
    id INTEGER PRIMARY KEY,
    username TEXT NOT NULL,
    identifier TEXT NOT NULL,
    private_key TEXT NOT NULL,
    wallet_balance REAL NOT NULL DEFAULT 100,
    difficulty INTEGER DEFAULT 1000 NOT NULL,
    password TEXT NOT NULL
)

CREATE TABLE notifications (
    node_id INTEGER NOT NULL,
    numberOfNotifications INTEGER DEFAULT 0,
    FOREIGN KEY (node_id) REFERENCES nodes(id)
)

CREATE TABLE sqlite_sequence(name,seq)

CREATE TABLE transactions
(
    id INTEGER PRIMARY KEY,
    sender TEXT NOT NULL,       
    reciever TEXT NOT NULL,
    amount REAL NOT NULL,
    FOREIGN KEY(sender) REFERENCES nodes(username),
    FOREIGN KEY(reciever) REFERENCES nodes(username)
)

CREATE TRIGGER check_block_approval_after_update
AFTER UPDATE ON node_approvals
FOR EACH ROW
BEGIN
UPDATE blocks
SET approved = (
    SELECT COUNT() >= (SELECT COUNT() FROM nodes) / 2
    FROM node_approvals
    WHERE block_id = NEW.block_id AND approved = TRUE
)
WHERE id = NEW.block_id;
END

CREATE TRIGGER insert_node_approvals_after_block_insert
AFTER INSERT ON blocks
BEGIN
    INSERT INTO node_approvals (node_id, block_id, approve)
    SELECT id AS node_id, NEW.id AS block_id, 0 AS approve
    FROM nodes;
END



INSERT DUMMY BLOCKS

GENESIS BLOCK : 
INSERT INTO blocks (previous_block_hash, block_hash, transaction_id)
VALUES (NULL, '445534234255', 1);

