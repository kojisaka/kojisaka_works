const TypeBus = "Bus";
const TypeFC  = "FC";
const PortNo = 80;
const SqlIns = "INSERT INTO dep_status (name,type,shared) VALUES (?,?,?)";
const SqlSel = "SELECT name,type,GROUP_CONCAT(shared) AS detail "
				+ "FROM dep_status GROUP BY name,type";
const DBProp = {
	host    : "localhost",
	user    : "root",
	password: "pass",
	database: "edu_sample"
};

const express = require("express");
const mySql = require("mysql2/promise");
const app = express();
const htmlRegMem = require("fs").readFileSync("./regMem.html", "utf8");
const htmlRegDB  = require("fs").readFileSync("./regDB.html" , "utf8");
const htmlDisp   = require("fs").readFileSync("./disp.html"  , "utf8");
const dss = [];

app.use(express.urlencoded({ extended: true }));

app.get("/regMem", async (req, res) => {
	sendRegHtml(htmlRegMem, res, "", "", "", "");
});

app.post("/regMem", async (req, res) => {
	let name   = req.body.name  ;
	let type   = req.body.type  ;
	let detail = req.body.detail;
	let msg;
	if (name === null || name.match(".*[\0-/].*")) {
		msg = "名前の入力が正しくありません！";
	} else if (type === TypeBus) {
		if (! detail.match("[0-9]+")) {
			msg = "利用者数の入力が正しくありません！";
		} else {
			let num = Number.parseInt(detail);
			let ds = new BusDepStatus(name, type, num);
			dss.push(ds);
			msg = name + " を登録しました。";
			detail = type = name = "";
		}
	} else if (type === TypeFC) {
		let cs = detail.trim().split(" +");
		let ds = new FCDepStatus(name, type, cs);
		dss.push(ds);
		msg = name + " を登録しました。";
		detail = type = name = "";
	} else {
		msg = "業種の入力が正しくありません！";
	}
	sendRegHtml(htmlRegMem, res, name, type, detail, msg);
});

app.post("/dispMem", async (req, res) => {
	let num_of_rslt = dss.length;
	let rslt_list = "";
	for (const ds of dss) {
		rslt_list += "\t" + ds.getStatus() + "<br>\n";
	}
	res.type("html").send(htmlDisp.replaceAll("${num_of_rslt}", num_of_rslt)
								  .replaceAll("${rslt_list}"  , rslt_list  ));
});

app.get("/regDB", async (req, res) => {
	sendRegHtml(htmlRegDB, res, "", "", "", "");
});

app.post("/regDB", async (req, res) => {
	let name   = req.body.name  ;
	let type   = req.body.type  ;
	let detail = req.body.detail;
	let msg;
	let con = null;
	try {
		if (name === null || name.match(".*[\0-/].*")) {
			msg = "名前の入力が正しくありません！";
		} else if (type === TypeBus) {
			if (! detail.match("[0-9]+")) {
				msg = "利用者数の入力が正しくありません！";
			} else {
				con = await mySql.createConnection(DBProp);
				await con.query(SqlIns, [name, type, detail]);
				msg = name + " を登録しました。";
				detail = type = name = "";
			}
		} else if (type === TypeFC) {
			const cs = detail.trim().split(/ +/);
			con = await mySql.createConnection(DBProp);
			for (const c of cs) {
				await con.query(SqlIns, [name, type, c]);
			}
			msg = name + " を登録しました。";
			detail = type = name = "";
		} else {
			msg = "業種の入力が正しくありません！";
		}
	} catch(e) {
		msg = "想定外の異常が発生しました！　" + e.message;
	} finally {
		if (con !== null) con.end();
	}
	sendRegHtml(htmlRegDB, res, name, type, detail, msg);
});

app.post("/dispDB", async (req, res) => {
	let con = null;
	try {
		con = await mySql.createConnection(DBProp);
		const [rs] = await con.query(SqlSel);
		const num_of_rslt = String(rs.length);
		let rslt_list = "";
		for (const r of rs) {
			rslt_list += "\t" + r.name + "　" + r.type + "　" + r.detail + "<br>\n";
		}
		res.type("html").send(htmlDisp.replaceAll("${num_of_rslt}", num_of_rslt)
									  .replaceAll("${rslt_list}"  , rslt_list  ));
	} catch(e) {
	} finally {
		if (con !== null) con.end();
	}
});

function sendRegHtml(html, res, name, type, detail, msg) {
	res.type("html").send(html.replaceAll("${name}"  , name  )
							  .replaceAll("${type}"  , type  )
							  .replaceAll("${detail}", detail)
							  .replaceAll("${msg}"   , msg   ));
}

class DepStatus {

	name;
	type;

	constructor(n, t) {
		this.name = n;
		this.type = t;
	}

	getStatus() {
		return null;
	}
}

class BusDepStatus extends DepStatus {

	numOfPAX;

	constructor(n, t, num) {
		super(n, t);
		this.numOfPAX = num;
	}

	getStatus() {
		return this.name + "　バス　利用者数: " + this.numOfPAX;
	}
}

class FCDepStatus extends DepStatus {

	chains = new Array(0);

	constructor(n, t, cs) {
		super(n, t);
		this.chains = cs;
	}

	getStatus() {
		let status = this.name + "　FC　店一覧: ";
		for (const chain of this.chains) {
			status += (chain + " ");
		}
		return status;
	}
}

app.listen(PortNo, () => console.log("Listening on port " + PortNo))
