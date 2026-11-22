package com.example.demo;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.Arrays;
import java.util.List;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;

@Controller
public class Test {

	final static String TypeBus = "Bus";
	final static String TypeFC  = "FC";
	final static String Sql
		= "INSERT INTO dep_status (name,type,shared) VALUES (?,?,?)";

	@GetMapping("/Test")
	public String get(Model m) {
		m.addAttribute("name"  , "");
		m.addAttribute("type"  , "");
		m.addAttribute("detail", "");
		m.addAttribute("msg"   , "");
		return "regDB.html";
	}

	@PostMapping("/Test")
	public String post
		(@RequestParam String name,
		 @RequestParam String type,
		 @RequestParam String detail,
		 Model m) {
		String msg = "";
		Connection con = null;
		Statement stmt = null;
		ResultSet rs = null;
		PreparedStatement ps = null;
		try {
			con = DriverManager.getConnection(DBProps.url,
												DBProps.user, DBProps.pass);
			if (name.isEmpty() || name.matches(".*[\0-/].*")) {
				msg = "名前の入力が正しくありません！";
			} else if (type.equals(TypeBus)) {
				if (! detail.matches("[0-9]+")) {
					msg = "利用者数の入力が正しくありません！";
				} else {
					con.setAutoCommit(false);
					stmt = con.createStatement();
					String sqlSel = "SELECT id,shared "
						+ "FROM dep_status WHERE "
						+ "name='" + name + "' AND type='" + type + "'";
					rs = stmt.executeQuery(sqlSel);
					rs.next();
					int id = rs.getInt("id");
					String shared = rs.getString("Shared");
					try {Thread.sleep(3000); } catch(Exception e) { }
					int num	= Integer.parseInt(shared);
					int adj = Integer.parseInt(detail);
					String sqlUpd = "UPDATE dep_status "
							+ "SET shared = ? WHERE id=" + id;
					ps = con.prepareStatement(sqlUpd);
					ps.setString(1, String.valueOf(num + adj));
					ps.executeUpdate();
					con.commit();
					msg = name + " を登録しました。";
					detail = type = name = "";
				}
			} else if (type.equals(TypeFC)) {
				List<String> cs
					= Arrays.asList(detail.trim().split(" +"));
				ps = con.prepareStatement(Sql);
				for (String c: cs) {
					ps.setString(1, name);
					ps.setString(2, type);
					ps.setString(3, c);
					ps.executeUpdate();
				}
				msg = name + " を登録しました。";
				detail = type = name = "";
			} else {
				msg = "業種の入力が正しくありません！";
			}
		} catch(Exception e) {
			e.getStackTrace();
			StackTraceElement tf = e.getStackTrace()[1];
			msg = "想定外の異常が発生しました！　"
					+ tf.getClassName() + "　" + tf.getMethodName()
					+ ". " + e.getMessage();
		} finally {
			try {
				if (ps   != null) ps.close();
				if (stmt != null) stmt.close();
				if (con  != null) con.close();
			} catch(Exception e) {
				e.getStackTrace();
			}
		}
		m.addAttribute("name"  , name  );
		m.addAttribute("type"  , type  );
		m.addAttribute("detail", detail);
		m.addAttribute("msg"   , msg   );
		return "regDB.html";
	}
}
