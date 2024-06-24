CREATE TABLE blockchain (
    id INTEGER PRIMARY KEY,
    prev_hash TEXT NOT NULL,
    hash TEXT NOT NULL,
    FOREIGN KEY(prev_hash) REFERENCES blocks(previous_block_hash)
    FOREIGN KEY (hash) REFERENCES blocks(block_hash)
)

CREATE TABLE "blocks" (
    id INTEGER PRIMARY KEY,
    previous_block_hash TEXT,
    block_hash TEXT NOT NULL,
    transaction_id INTEGER NOT NULL UNIQUE,
    time_stamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(transaction_id) REFERENCES transactions(id)
)

CREATE TABLE "miners" (
	"miner_id"	INTEGER,
	"wallet_balance"	REAL NOT NULL,
	FOREIGN KEY("wallet_balance") REFERENCES "nodes"("wallet_balance")
)

CREATE TABLE "node_approvals" (
	"id"	INTEGER,
	"node_id"	INT,
	"transaction_id"	INT,
	"approve"	INT DEFAULT 0 CHECK("approve" IN (0, 1)),
	FOREIGN KEY("node_id") REFERENCES "nodes"("id"),
	FOREIGN KEY("transaction_id") REFERENCES "transactions"("id"),
	PRIMARY KEY("id")
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

CREATE TABLE transactions
(
    id INTEGER PRIMARY KEY,
    sender TEXT NOT NULL,       
    reciever TEXT NOT NULL,
    amount REAL NOT NULL, approved INTEGER DEFAULT 0 CHECK (approved IN (0, 1)),
    FOREIGN KEY(sender) REFERENCES nodes(username),
    FOREIGN KEY(reciever) REFERENCES nodes(username)
)

CREATE TRIGGER insert_node_approvals_after_transaction_insert
AFTER INSERT ON transactions
BEGIN
    INSERT INTO node_approvals (node_id, transaction_id, approve)
    SELECT id AS node_id, NEW.id AS transaction_id, 0 AS approve
    FROM nodes;
END

CREATE TRIGGER update_receiver_wallet_balance
AFTER UPDATE OF approved ON transactions
FOR EACH ROW
WHEN NEW.approved = 1
BEGIN
    -- Update receiver's wallet balance
    UPDATE nodes
    SET wallet_balance = wallet_balance + (SELECT amount FROM transactions WHERE id = NEW.id)
    WHERE username = new.reciever;
	
    -- Update sender's wallet balance
    UPDATE nodes
    SET wallet_balance = wallet_balance - (SELECT amount FROM transactions WHERE id = NEW.id)
    WHERE username = new.sender;
END

